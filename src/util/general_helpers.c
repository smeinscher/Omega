//
// Created by sterling on 7/29/24.
//

#include "general_helpers.h"
#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Coords
{
    int q;
    int r;
} Coords;

typedef struct Node
{
    Coords position;
    struct Node *parent;
    struct Node *neighbors[6];
    int g;
    int h;
    int f;
} Node;

static int g_directions[6][2] = {{0, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 0}, {1, -1}};

void realloc_int(int **int_ptr, size_t size)
{
    int *temp_int_ptr = realloc(*int_ptr, size * sizeof(int));
    if (temp_int_ptr == NULL)
    {
        printf("Error reallocating memory for board_unit_owner\n");
        return;
    }
    *int_ptr = temp_int_ptr;
}

void realloc_float(float **float_ptr, size_t size)
{
    float *temp_float_ptr = realloc(*float_ptr, size * sizeof(float));
    if (temp_float_ptr == NULL)
    {
        printf("Error reallocating memory for board_unit_owner\n");
        return;
    }
    *float_ptr = temp_float_ptr;
}

int max_int(int a, int b)
{
    return a > b ? a : b;
}

void hex_grid_axial_to_offset(int q, int r, int *x, int *y)
{
    *x = q;
    *y = r + (q - (q & 1)) / 2;
}

void hex_grid_offset_to_axial(int x, int y, int *q, int *r)
{
    *q = x;
    *r = y - (x - (x & 1)) / 2;
}

int hex_grid_get_axial_distance(int start_q, int start_r, int target_q, int target_r)
{
    int start_s = -start_q - start_r;
    int target_s = -target_q - target_r;
    int sub_q = start_q - target_q;
    int sub_r = start_r - target_r;
    int sub_s = start_s - target_s;
    return max_int(abs(sub_s), max_int(abs(sub_q), abs(sub_r)));
}

void hex_grid_rotation_get_next(bool clockwise, int distance_from_center, int *q, int *r)
{
    int s = -(*q) - *r;
    if (distance_from_center > 0)
    {
        if (!clockwise)
        {
            if (*q > -distance_from_center && *r < 0 && s == distance_from_center)
            {
                (*q)--;
                (*r)++;
            }
            else if (*q == -distance_from_center && *r < distance_from_center && s > 0)
            {
                (*r)++;
            }
            else if (*q < 0 && *r == distance_from_center && s > -distance_from_center)
            {
                (*q)++;
            }
            else if (*q < distance_from_center && *r > 0 && s == -distance_from_center)
            {
                (*q)++;
                (*r)--;
            }
            else if (*q == distance_from_center && *r > -distance_from_center && s < 0)
            {
                (*r)--;
            }
            else
            {
                (*q)--;
            }
        }
        else
        {
            // TODO: implement this
            printf("Not implemented\n");
        }
    }
}

DynamicIntArray *hex_grid_find_path(Board *board, int start_x, int start_y, int target_x, int target_y,
                                    int board_dimension_x, int board_dimension_y)
{
    if (start_x < 0 || start_x >= board_dimension_x || start_y < 0 || start_y >= board_dimension_y || target_x < 0 ||
        target_x >= board_dimension_x || target_y < 0 || target_y >= board_dimension_y)
    {
        printf("Doh\n");
        return NULL;
    }
    bool target_is_occupied = board_tile_is_occupied(board, target_x, target_y);
    int *path = malloc(sizeof(int) * board_dimension_x * board_dimension_y * 2);
    memset(path, 0, sizeof(int) * board_dimension_x * board_dimension_y * 2);

    Node *all_nodes = malloc(sizeof(Node) * board_dimension_x * board_dimension_y);
    for (int i = 0; i < board_dimension_x * board_dimension_y; i++)
    {
        int q, r;
        hex_grid_offset_to_axial(i % board_dimension_x, i / board_dimension_x, &q, &r);
        all_nodes[i].position.q = q;
        all_nodes[i].position.r = r;
        all_nodes[i].parent = NULL;
        for (int j = 0; j < 6; j++)
        {
            int new_q = all_nodes[i].position.q + g_directions[j][0];
            int new_r = all_nodes[i].position.r + g_directions[j][1];
            int x, y;
            hex_grid_axial_to_offset(new_q, new_r, &x, &y);
            if (x < 0 || x >= board_dimension_x || y < 0 || y >= board_dimension_y)
            {
                all_nodes[i].neighbors[j] = NULL;
            }
            else
            {
                all_nodes[i].neighbors[j] = &all_nodes[y * board_dimension_x + x];
            }
        }
        all_nodes[i].g = 0;
        all_nodes[i].f = 0;
        all_nodes[i].h = 0;
    }

    Node **to_search = malloc(sizeof(Node *) * board_dimension_x * board_dimension_y);
    memset(to_search, 0, sizeof(Node *) * board_dimension_x * board_dimension_y);
    to_search[0] = &all_nodes[start_y * board_dimension_x + start_x];
    int to_search_count = 1;
    int to_search_index = 0;

    Node **processed = malloc(sizeof(Node *) * board_dimension_x * board_dimension_y);
    memset(processed, 0, sizeof(Node *) * board_dimension_x * board_dimension_y);
    int processed_count = 0;

    int iterations = 0;
    while (to_search_count > 0)
    {
        iterations++;
        Node *current = NULL;
        int current_index;
        for (int i = 0; i < to_search_count; i++)
        {
            if (to_search[i] != NULL)
            {
                current = to_search[i];
                current_index = i;
                break;
            }
        }
        if (current == NULL)
        {
            printf("Error: current is null\nIterations: %d\n", iterations);
            break;
        }
        for (int i = to_search_index; i < to_search_count; i++)
        {
            if (to_search[i] == NULL)
            {
                continue;
            }
            if (to_search[i]->f < current->f || to_search[i]->f == current->f && to_search[i]->h < current->h)
            {
                current = to_search[i];
                current_index = i;
            }
        }

        processed[processed_count++] = current;
        to_search[current_index] = NULL;

        // TODO: double check comparison
        if (current->position.q == all_nodes[target_y * board_dimension_x + target_x].position.q &&
            current->position.r == all_nodes[target_y * board_dimension_x + target_x].position.r)
        {
            Node *current_path_tile = &all_nodes[target_y * board_dimension_x + target_x];
            if (target_is_occupied)
            {
                current_path_tile = current_path_tile->parent;
            }
            int count = 0;
            while (current_path_tile != NULL &&
                   (current_path_tile->position.q != all_nodes[start_y * board_dimension_x + start_x].position.q ||
                    current_path_tile->position.r != all_nodes[start_y * board_dimension_x + start_x].position.r))
            {
                int x, y;
                hex_grid_axial_to_offset(current_path_tile->position.q, current_path_tile->position.r, &x, &y);
                path[count * 2] = x;
                path[count * 2 + 1] = y;
                current_path_tile = current_path_tile->parent;
                if (current_path_tile == NULL)
                {
                    break;
                }
                count++;
                if (count > board_dimension_x * board_dimension_y - 1)
                {
                    printf("Error generating path\n");
                    free(path);
                    free(all_nodes);
                    free(to_search);
                    free(processed);
                    return NULL;
                }
            }
            DynamicIntArray *return_path = malloc(sizeof(DynamicIntArray));
            da_int_init(return_path, count * 2);
            for (int i = 0; i < count; i++)
            {
                da_int_push_back(return_path, path[i * 2]);
                da_int_push_back(return_path, path[i * 2 + 1]);
            }
            free(path);
            free(all_nodes);
            free(to_search);
            free(processed);
            return return_path;
        }

        for (int i = 0; i < 6; i++)
        {
            if (current->neighbors[i] == NULL)
            {
                continue;
            }
            int x, y;
            hex_grid_axial_to_offset(current->neighbors[i]->position.q, current->neighbors[i]->position.r, &x, &y);
            // TODO: check if valid tile (unit in the way, non-passable tile, etc...)
            if (board_tile_is_occupied(board, x, y) && (x != target_x || y != target_y))
            {
                continue;
            }

            bool in_processed = false;
            for (int j = 0; j < processed_count; j++)
            {
                if (processed[j] == NULL)
                {
                    continue;
                }
                // TODO: double check if this comparison works
                if (processed[j]->position.q == current->neighbors[i]->position.q &&
                    processed[j]->position.r == current->neighbors[i]->position.r)
                {
                    in_processed = true;
                    break;
                }
            }
            if (in_processed)
            {
                continue;
            }

            bool in_to_search = false;
            for (int j = 0; j < to_search_count; j++)
            {
                if (to_search[j] == NULL)
                {
                    continue;
                }
                // TODO: double check if this comparison works
                if (to_search[j]->position.q == current->neighbors[i]->position.q &&
                    to_search[j]->position.r == current->neighbors[i]->position.r)
                {
                    in_to_search = true;
                    break;
                }
            }
            int cost = current->g + 1;
            if (!in_to_search || cost < current->neighbors[i]->g)
            {
                current->neighbors[i]->g = cost;
                current->neighbors[i]->parent = current;

                if (!in_to_search)
                {
                    int target_q, target_r;
                    hex_grid_offset_to_axial(target_x, target_y, &target_q, &target_r);
                    current->neighbors[i]->h = hex_grid_get_axial_distance(
                        current->neighbors[i]->position.q, current->neighbors[i]->position.r, target_q, target_r);
                    to_search[to_search_count++] = current->neighbors[i];
                }
            }
        }
    }

    printf("Failed to find path\n");
    free(path);
    free(all_nodes);
    free(to_search);
    free(processed);
    return NULL;
}

Node *hex_grid_tile_fill_neighbors(Board *board, int q, int r, float movement_points)
{
    int x, y;
    hex_grid_axial_to_offset(q, r, &x, &y);
    if (movement_points < 0.0f || board_tile_is_occupied(board, x, y))
    {
        return NULL;
    }
    Node *node = malloc(sizeof(Node));
    node->position.q = q;
    node->position.r = r;
    for (int i = 0; i < 6; i++)
    {
        // TODO: replace 1.0f with actual movement cost
        node->neighbors[i] =
            hex_grid_tile_fill_neighbors(board, q + g_directions[i][0], r + g_directions[i][1], movement_points - 1.0f);
    }
    return node;
}

void hex_grid_fill_possible_tiles(DynamicIntArray *possible_tiles, Node *node)
{
    if (node == NULL)
    {
        return;
    }
    int x, y;
    hex_grid_axial_to_offset(node->position.q, node->position.r, &x, &y);
    bool in_possible_tiles = false;
    for (int i = 0; i < possible_tiles->used; i += 2)
    {
        if (possible_tiles->array[i] == x && possible_tiles->array[i + 1] == y)
        {
            in_possible_tiles = true;
        }
    }
    if (!in_possible_tiles)
    {
        da_int_push_back(possible_tiles, x);
        da_int_push_back(possible_tiles, y);
    }
    for (int i = 0; i < 6; i++)
    {
        hex_grid_fill_possible_tiles(possible_tiles, node->neighbors[i]);
    }
}

void free_nodes(Node *node)
{
    if (node == NULL)
    {
        return;
    }
    for (int i = 0; i < 6; i++)
    {
        free_nodes(node->neighbors[i]);
    }
    free(node);
}

DynamicIntArray *hex_grid_possible_moves(Board *board, int unit_index, int unit_x, int unit_y)
{
    DynamicIntArray *possible_tiles = malloc(sizeof(DynamicIntArray));
    // TODO: replace 36 with variable based on movement points
    da_int_init(possible_tiles, 36);

    int start_q, start_r;
    hex_grid_offset_to_axial(unit_x, unit_y, &start_q, &start_r);

    float initial_movement_points =
        board->units->unit_type[unit_index] != STATION ? board->units->unit_movement_points[unit_index] : 2.0f;
    Node *neighbors[6];
    for (int i = 0; i < 6; i++)
    {
        neighbors[i] = hex_grid_tile_fill_neighbors(board, start_q + g_directions[i][0], start_r + g_directions[i][1],
                                                    initial_movement_points - 1.0f);
    }

    for (int i = 0; i < 6; i++)
    {
        hex_grid_fill_possible_tiles(possible_tiles, neighbors[i]);
        free_nodes(neighbors[i]);
    }

    return possible_tiles;
}

Node *hex_grid_tile_fill_neighbors_include_enemies(Board *board, int unit_index, int q, int r, float movement_points)
{
    int x, y;
    hex_grid_axial_to_offset(q, r, &x, &y);
    if (movement_points < 0.0f || x < 0 || x >= board->board_dimension_x || y < 0 || y >= board->board_dimension_y)
    {
        return NULL;
    }
    if (board_tile_is_occupied(board, x, y))
    {
        int enemy_unit_index = board->units->unit_tile_occupation_status[y * board->board_dimension_x + x];
        if (board->units->unit_owner[enemy_unit_index] == board->units->unit_owner[unit_index])
        {
            return NULL;
        }
    }
    Node *node = malloc(sizeof(Node));
    node->position.q = q;
    node->position.r = r;
    for (int i = 0; i < 6; i++)
    {
        if (!board_tile_is_occupied(board, x, y))
        {
            // TODO: replace 1.0f with actual movement cost
            node->neighbors[i] = hex_grid_tile_fill_neighbors_include_enemies(
                board, unit_index, q + g_directions[i][0], r + g_directions[i][1], movement_points - 1.0f);
        }
        else
        {
            node->neighbors[i] = NULL;
        }
    }
    return node;
}

void hex_grid_fill_attackable_tiles(Board *board, int unit_index, DynamicIntArray *possible_tiles, Node *node)
{
    if (node == NULL)
    {
        return;
    }
    int x, y;
    hex_grid_axial_to_offset(node->position.q, node->position.r, &x, &y);
    bool in_possible_tiles = false;
    for (int i = 0; i < possible_tiles->used; i += 2)
    {
        if (possible_tiles->array[i] == x && possible_tiles->array[i + 1] == y)
        {
            in_possible_tiles = true;
        }
    }
    int enemy_unit_index = board->units->unit_tile_occupation_status[y * board->board_dimension_x + x];
    if (!in_possible_tiles && enemy_unit_index != -1 &&
        board->units->unit_owner[enemy_unit_index] != board->units->unit_owner[unit_index])
    {
        da_int_push_back(possible_tiles, x);
        da_int_push_back(possible_tiles, y);
    }
    for (int i = 0; i < 6; i++)
    {
        hex_grid_fill_attackable_tiles(board, unit_index, possible_tiles, node->neighbors[i]);
    }
}

DynamicIntArray *hex_grid_possible_attacks(Board *board, int unit_index, int unit_x, int unit_y)
{
    DynamicIntArray *possible_tiles = malloc(sizeof(DynamicIntArray));
    // TODO: replace 10 with something else
    da_int_init(possible_tiles, 10);

    int start_q, start_r;
    hex_grid_offset_to_axial(unit_x, unit_y, &start_q, &start_r);

    Node *neighbors[6];
    for (int i = 0; i < 6; i++)
    {
        neighbors[i] = hex_grid_tile_fill_neighbors_include_enemies(
            board, unit_index, start_q + g_directions[i][0], start_r + g_directions[i][1],
            board->units->unit_movement_points[unit_index] - 1.0f);
    }

    for (int i = 0; i < 6; i++)
    {
        hex_grid_fill_attackable_tiles(board, unit_index, possible_tiles, neighbors[i]);
        free_nodes(neighbors[i]);
    }
    return possible_tiles;
}
