//
// Created by sterling on 6/26/24.
//

#include "board.h"

#include "../players/players.h"
#include "../renderer/camera.h"
#include "../util/general_helpers.h"
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

static float hovered_tile_color[4] = {0.7f, 0.7f, 0.7f, 0.6f};
static float selected_tile_color[4] = {0.2f, 0.7f, 0.9f, 0.3f};
static float highlighted_tile_color[4] = {0.2f, 0.7f, 0.9f, 0.2f};
static float highlighted_tile_occupied_color[4] = {0.7f, 0.2f, 0.2f, 0.4f};
static float highlighted_tile_friendly_color[4] = {0.2f, 0.7f, 0.2f, 0.4f};
static float highlighted_tile_ready_color[4] = {0.7f, 0.7f, 0.4f, 0.4f};
static float highlighted_tile_path_color[4] = {0.7f, 0.2f, 0.7f, 0.4f};

static int coords_to_point(int x_pos, int y_pos, int dimension_x, int dimension_y)
{
    if (x_pos < 0 || x_pos >= dimension_x || y_pos < 0 || y_pos >= dimension_y)
    {
        return -1;
    }
    int row_start = y_pos * (dimension_x * 2 + 2) + x_pos % 2 * (dimension_x * 2 + 2);
    int row_offset = x_pos * 2 + (dimension_x * 2 + 2) * x_pos % 2;

    return row_start + row_offset;
}

float *board_vertices_create(int dimension_x, int dimension_y, int board_offset_x, int board_offset_y)
{
    unsigned int num_vertices = dimension_y * 2 + dimension_y * (dimension_x * 2) + dimension_x * 2 + 1;
    float *board_vertices = malloc(sizeof(float) * num_vertices * 2);
    if (board_vertices == NULL)
    {
        printf("Error allocating board_vertices\n");
        return NULL;
    }
    int index = 0;
    for (int i = 0; i < dimension_y; i++)
    {
        board_vertices[index++] = (float)board_offset_x;
        board_vertices[index++] =
            (float)board_offset_y + (float)i * board_get_hex_tile_height() + board_get_hex_tile_height() / 2.0f;
        for (int j = 0; j < dimension_x; j++)
        {
            const float y_offset = (float)(j % 2) * board_get_hex_tile_height() / 2.0f;
            board_vertices[index++] = (float)board_offset_x + (float)j * 3.0f * board_get_hex_tile_width() / 4.0f +
                                      board_get_hex_tile_width() / 4.0f;
            board_vertices[index++] = (float)board_offset_y + (float)i * board_get_hex_tile_height() + y_offset;
            board_vertices[index++] = (float)board_offset_x + (float)j * 3.0f * board_get_hex_tile_width() / 4.0f +
                                      3.0f * board_get_hex_tile_width() / 4.0f;
            board_vertices[index++] = (float)board_offset_y + (float)i * board_get_hex_tile_height() + y_offset;
        }
        board_vertices[index++] = (float)board_offset_x +
                                  (float)dimension_x * 3.0f * board_get_hex_tile_width() / 4.0f +
                                  board_get_hex_tile_width() / 4.0f;
        board_vertices[index++] = (float)board_offset_y + (float)i * board_get_hex_tile_height() +
                                  board_get_hex_tile_height() / (1.0f + (float)(dimension_x % 2));
    }
    board_vertices[index++] = (float)board_offset_x;
    board_vertices[index++] =
        (float)board_offset_y + (float)dimension_y * board_get_hex_tile_height() + board_get_hex_tile_height() / 2.0f;
    for (int j = 0; j < dimension_x; j++)
    {
        const float y_offset = (float)(j % 2) * board_get_hex_tile_height() / 2.0f;
        board_vertices[index++] = (float)board_offset_x + (float)(j) * 3.0f * board_get_hex_tile_width() / 4.0f +
                                  board_get_hex_tile_width() / 4.0f;
        board_vertices[index++] = (float)board_offset_y + (float)dimension_y * board_get_hex_tile_height() + y_offset;
        board_vertices[index++] = (float)board_offset_x + (float)j * 3.0f * board_get_hex_tile_width() / 4.0f +
                                  3.0f * board_get_hex_tile_width() / 4.0f;
        board_vertices[index++] = (float)board_offset_y + (float)dimension_y * board_get_hex_tile_height() + y_offset;
    }
    return board_vertices;
}

unsigned int *board_outline_indices_create(int dimension_x, int dimension_y)
{
    unsigned int num_vertices = dimension_y * dimension_x * 12;
    unsigned int *board_indices = malloc(sizeof(unsigned int) * num_vertices);
    if (board_indices == NULL)
    {
        printf("Error allocating board_indices\n");
        return NULL;
    }
    int index = 0;
    for (int i = 0; i < dimension_y; i++)
    {
        for (int j = 0; j < dimension_x; j++)
        {
            if (j % 2 == 0)
            {
                int start_point = i * (dimension_x * 2 + 2) + j * 2;
                // Top Left
                board_indices[index++] = start_point;
                board_indices[index++] = start_point + 1;
                // Top Center
                board_indices[index++] = start_point + 1;
                board_indices[index++] = start_point + 2;
                // Top Right
                board_indices[index++] = start_point + 2;
                board_indices[index++] = start_point + 3;
                // Bottom Right
                board_indices[index++] = start_point + 3;
                board_indices[index++] = start_point + (dimension_x * 2 + 2) + 2;
                // Bottom Center
                board_indices[index++] = start_point + (dimension_x * 2 + 2) + 2;
                board_indices[index++] = start_point + (dimension_x * 2 + 2) + 1;
                // Bottom Left
                board_indices[index++] = start_point + (dimension_x * 2 + 2) + 1;
                board_indices[index++] = start_point;
            }
            else
            {
                int start_point = i * (dimension_x * 2 + 2) + j * 2 + (dimension_x * 2 + 2);
                // Top Left
                board_indices[index++] = start_point;
                board_indices[index++] = start_point - (dimension_x * 2 + 2) + 1;
                // Top Center
                board_indices[index++] = start_point - (dimension_x * 2 + 2) + 1;
                board_indices[index++] = start_point - (dimension_x * 2 + 2) + 2;
                // Top Right
                board_indices[index++] = start_point - (dimension_x * 2 + 2) + 2;
                board_indices[index++] = start_point + 3;
                // Bottom Right
                board_indices[index++] = start_point + 3;
                board_indices[index++] = start_point + 2;
                // Bottom Center
                board_indices[index++] = start_point + 2;
                board_indices[index++] = start_point + 1;
                // Bottom Left
                board_indices[index++] = start_point + 1;
                board_indices[index++] = start_point;
            }
        }
    }
    return board_indices;
}

void board_set_tile_border_vertices(Board *board, int x, int y, int border_type, int start_index)
{
    board->board_border_positions[start_index] = (float)x * board_get_hex_tile_width() * 0.75f;
    board->board_border_positions[start_index + 1] = (float)y * board_get_hex_tile_height() +
                                                     (float)(x % 2) * board_get_hex_tile_height() / 2.0f +
                                                     board_get_hex_tile_height();
    board->board_border_positions[start_index + 2] =
        (float)x * board_get_hex_tile_width() * 0.75f + board_get_hex_tile_width();
    board->board_border_positions[start_index + 3] =
        (float)y * board_get_hex_tile_height() + (float)(x % 2) * board_get_hex_tile_height() / 2.0f;
    board->board_border_positions[start_index + 4] = (float)x * board_get_hex_tile_width() * 0.75f;
    board->board_border_positions[start_index + 5] =
        (float)y * board_get_hex_tile_height() + (float)(x % 2) * board_get_hex_tile_height() / 2.0f;
    board->board_border_positions[start_index + 6] = (float)x * board_get_hex_tile_width() * 0.75f;
    board->board_border_positions[start_index + 7] = (float)y * board_get_hex_tile_height() +
                                                     (float)(x % 2) * board_get_hex_tile_height() / 2.0f +
                                                     board_get_hex_tile_height();
    board->board_border_positions[start_index + 8] =
        (float)x * board_get_hex_tile_width() * 0.75f + board_get_hex_tile_width();
    board->board_border_positions[start_index + 9] =
        (float)y * board_get_hex_tile_height() + (float)(x % 2) * board_get_hex_tile_height() / 2.0f;
    board->board_border_positions[start_index + 10] =
        (float)x * board_get_hex_tile_width() * 0.75f + board_get_hex_tile_width();
    board->board_border_positions[start_index + 11] = (float)y * board_get_hex_tile_height() +
                                                      (float)(x % 2) * board_get_hex_tile_height() / 2.0f +
                                                      board_get_hex_tile_height();

    board->board_border_uvs[start_index] = (float)border_type / 6.0f;
    board->board_border_uvs[start_index + 1] = 1.0f;
    board->board_border_uvs[start_index + 2] = ((float)border_type + 1.0f) / 6.0f;
    board->board_border_uvs[start_index + 3] = 0.0f;
    board->board_border_uvs[start_index + 4] = (float)border_type / 6.0f;
    board->board_border_uvs[start_index + 5] = 0.0f;
    board->board_border_uvs[start_index + 6] = (float)border_type / 6.0f;
    board->board_border_uvs[start_index + 7] = 1.0f;
    board->board_border_uvs[start_index + 8] = ((float)border_type + 1.0f) / 6.0f;
    board->board_border_uvs[start_index + 9] = 0.0f;
    board->board_border_uvs[start_index + 10] = ((float)border_type + 1.0f) / 6.0f;
    board->board_border_uvs[start_index + 11] = 1.0f;

    float color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    switch (board->units->unit_tile_ownership_status[y * board->board_dimension_x + x])
    {
    case 1:
        color[0] = 0.7f;
        break;
    case 2:
        color[1] = 0.7f;
        break;
    case 3:
        color[1] = 0.7f;
        color[2] = 0.7f;
        break;
    case 4:
        color[0] = 0.7f;
        color[1] = 0.7f;
        break;
    default:
        break;
    }
    for (int i = 0; i < 6; i++)
    {
        board->board_border_colors[start_index * 2 + i * 4] = color[0];
        board->board_border_colors[start_index * 2 + i * 4 + 1] = color[1];
        board->board_border_colors[start_index * 2 + i * 4 + 2] = color[2];
        board->board_border_colors[start_index * 2 + i * 4 + 3] = color[3];
    }
}

// TODO: Error checking for malloc
Board *board_create(int dimension_x, int dimension_y, int player_count)
{
    Board *board = malloc(sizeof(Board));
    if (board == NULL)
    {
        printf("Error allocating board\n");
        return NULL;
    }
    board->board_dimension_x = dimension_x;
    board->board_dimension_y = dimension_y;
    board->player_count = player_count;
    board->hovered_point = -1;
    board->selected_point = -1;
    board->mouse_tile_index_x = -1;
    board->mouse_tile_index_y = -1;
    board->selected_tile_index_x = -1;
    board->selected_tile_index_y = -1;
    board->last_selected_tile_index_x = -1;
    board->last_selected_tile_index_y = -1;
    board->board_borders_count = 0;
    board->board_update_flags = 0;
    board->board_current_turn = 0;
    board->board_fill_positions = NULL;
    board->board_fill_colors = NULL;
    board->board_outline_vertices = NULL;
    board->board_outline_indices = NULL;
    board->board_border_positions = NULL;
    board->board_border_uvs = NULL;
    board->board_border_colors = NULL;
    board->board_highlighted_path = NULL;
    board->board_moveable_tiles = NULL;
    board->board_attackable_tiles = NULL;
    board->board_swapable_tiles = NULL;
    board->units = NULL;
    board->planets = NULL;

    board->board_outline_vertices = board_vertices_create(dimension_x, dimension_y, 0, 0);
    if (board->board_outline_vertices == NULL)
    {
        board_destroy(board);
        return NULL;
    }
    board->board_outline_indices = board_outline_indices_create(dimension_x, dimension_y);
    if (board->board_outline_indices == NULL)
    {
        board_destroy(board);
        return NULL;
    }

    board->board_borders_count = dimension_x * dimension_y * 6;

    board->board_border_positions = malloc(sizeof(float) * 12 * board->board_borders_count);
    if (board->board_border_positions == NULL)
    {
        printf("Error allocating board_border_positions\n");
        board_destroy(board);
        return NULL;
    }

    board->board_border_uvs = malloc(sizeof(float) * 12 * board->board_borders_count);
    if (board->board_border_uvs == NULL)
    {
        printf("Error allocating board_border_uvs\n");
        board_destroy(board);
        return NULL;
    }

    board->board_border_colors = malloc(sizeof(float) * 24 * board->board_borders_count);
    if (board->board_border_colors == NULL)
    {
        printf("Error allocating board_border_colors\n");
        board_destroy(board);
        return NULL;
    }

    // Each tile (hex) has 4 inner triangles, each with 3 vertices. 12 total vertices to store;
    unsigned int num_vertices = dimension_y * dimension_x * 12;
    // 2 position values per vertex
    board->board_fill_positions = malloc(sizeof(float) * num_vertices * 2);
    if (board->board_fill_positions == NULL)
    {
        printf("Error allocating board_fill_positions\n");
        board_destroy(board);
        return NULL;
    }
    memset(board->board_fill_positions, 0, sizeof(float) * num_vertices * 2);
    // 4 color values per vertex
    board->board_fill_colors = malloc(sizeof(float) * num_vertices * 4);
    if (board->board_fill_colors == NULL)
    {
        printf("Error allocating board_fill_colors\n");
        board_destroy(board);
        return NULL;
    }
    memset(board->board_fill_colors, 0, sizeof(float) * num_vertices * 4);

    board->board_moveable_tiles = malloc(sizeof(DynamicIntArray));
    // TODO: make this not a random number
    da_int_init(board->board_moveable_tiles, 36);

    board->board_attackable_tiles = malloc(sizeof(DynamicIntArray));
    da_int_init(board->board_attackable_tiles, 10);

    board->board_swapable_tiles = malloc(sizeof(DynamicIntArray));
    da_int_init(board->board_swapable_tiles, 10);

    board->units = units_create(board->board_dimension_x, board->board_dimension_y);
    if (board->units == NULL)
    {
        printf("Error creating units\n");
        board_destroy(board);
        return NULL;
    }

    board_update_border(board);

    // TODO: account for possibility that dimension_x could be greater than dimension_y, implement min_int
    board->planets = planets_create(board->board_dimension_x, board->board_dimension_y, dimension_x / 2 - 2);
    if (board->planets == NULL)
    {
        printf("Error creating planets\n");
        board_destroy(board);
        return NULL;
    }

    return board;
}

void board_reset(Board **board)
{
    int dimension_x = (*board)->board_dimension_x;
    int dimension_y = (*board)->board_dimension_y;
    int player_count = (*board)->player_count;
    board_destroy(*board);
    *board = board_create(dimension_x, dimension_y, player_count);
}

void board_reset_new_dimensions(Board **board, int new_board_dimension_x, int new_board_dimension_y)
{
    board_destroy(*board);
    *board = board_create(new_board_dimension_x, new_board_dimension_y, 0);
}

bool in_board_attackable_tiles(Board *board, int move_x, int move_y)
{
    for (int i = 0; i < board->board_attackable_tiles->used; i += 2)
    {
        int x = board->board_attackable_tiles->array[i];
        int y = board->board_attackable_tiles->array[i + 1];
        if (x == move_x && y == move_y)
        {
            return true;
        }
    }
    return false;
}

bool in_board_moveable_tiles(Board *board, int move_x, int move_y)
{
    if (board->board_moveable_tiles != NULL)
    {

        for (int i = 0; i < board->board_moveable_tiles->used; i += 2)
        {
            int x = board->board_moveable_tiles->array[i];
            int y = board->board_moveable_tiles->array[i + 1];
            if (x == move_x && y == move_y)
            {
                return true;
            }
        }
    }
    else
    {
        printf("yikes\n");
    }
    return false;
}

bool in_board_swapable_tiles(Board *board, int move_x, int move_y)
{
    if (board->board_swapable_tiles != NULL)
    {

        for (int i = 0; i < board->board_swapable_tiles->used; i += 2)
        {
            int x = board->board_swapable_tiles->array[i];
            int y = board->board_swapable_tiles->array[i + 1];
            if (x == move_x && y == move_y)
            {
                return true;
            }
        }
    }
    else
    {
        printf("yikes\n");
    }
    return false;
}

void board_handle_tile_placement(Board *board, UnitType unit_type, int player_count, int *score)
{
    if (in_board_moveable_tiles(board, board->mouse_tile_index_x, board->mouse_tile_index_y))
    {
        int player_index = board->board_current_turn % player_count;
        unit_purchase_with_score(board->units, player_index, score, unit_type, board->mouse_tile_index_x,
                                 board->mouse_tile_index_y, board->board_dimension_x, board->board_dimension_y);
    }
    da_int_clear(board->board_moveable_tiles);
}

void board_handle_tile_click(Board *board)
{
    if (board->board_highlighted_path != NULL)
    {
        da_int_free(board->board_highlighted_path);
        free(board->board_highlighted_path);
        board->board_highlighted_path = NULL;
    }
    if (board->selected_point == -1)
    {
        if (board->hovered_point == -1)
        {
            return;
        }
        board->selected_point = board->hovered_point;
        board->last_selected_tile_index_x = board->selected_tile_index_x;
        board->last_selected_tile_index_y = board->selected_tile_index_y;
        board->selected_tile_index_x = board->mouse_tile_index_x;
        board->selected_tile_index_y = board->mouse_tile_index_y;
        int unit_index =
            board->units->unit_tile_occupation_status[board->selected_tile_index_y * board->board_dimension_x +
                                                      board->selected_tile_index_x];
        if (unit_index == -1 ||
            board->units->unit_owner[unit_index] != board->board_current_turn % board->player_count + 1 ||
            board->units->unit_type[unit_index] == STATION)
        {
            return;
        }

        da_int_free(board->board_moveable_tiles);
        free(board->board_moveable_tiles);
        board->board_moveable_tiles =
            hex_grid_possible_moves(board, unit_index, board->mouse_tile_index_x, board->mouse_tile_index_y);

        if (board->units->unit_type[unit_index] != WORKER)
        {
            da_int_free(board->board_attackable_tiles);
            free(board->board_attackable_tiles);
            board->board_attackable_tiles =
                hex_grid_possible_attacks(board, unit_index, board->mouse_tile_index_x, board->mouse_tile_index_y);
        }

        da_int_free(board->board_swapable_tiles);
        free(board->board_swapable_tiles);
        board->board_swapable_tiles =
            hex_grid_possible_swaps(board, unit_index, board->mouse_tile_index_x, board->mouse_tile_index_y);
    }
    else
    {
        int unit_index =
            board->units->unit_tile_occupation_status[board->selected_tile_index_y * board->board_dimension_x +
                                                      board->selected_tile_index_x];
        // TODO: mouse tile index uninitialized?
        int move_x = board->mouse_tile_index_x;
        int move_y = board->mouse_tile_index_y;
        if (unit_index != -1 && move_x != -1 && move_y != -1)
        {
            bool can_attack = in_board_attackable_tiles(board, move_x, move_y);
            bool can_swap = in_board_swapable_tiles(board, move_x, move_y);
            bool can_move = in_board_moveable_tiles(board, move_x, move_y);
            if (can_attack)
            {
                //                int enemy_unit_index =
                //                    board->units->unit_tile_occupation_status[move_y * board->board_dimension_x +
                //                    move_x];
                //                board_process_attack(board, enemy_unit_index, unit_index);
                unit_add_movement_animation(board->units, unit_index, board->selected_tile_index_x,
                                            board->selected_tile_index_y, move_x, move_y, ATTACK);
                unit_cancel_action(board->units, unit_index);
            }
            else if (can_swap)
            {
                unit_swap(board->units, unit_index,
                          board->units->unit_tile_occupation_status[move_y * board->board_dimension_x + move_x],
                          board->selected_tile_index_x, board->selected_tile_index_y, move_x, move_y,
                          board->board_dimension_x, true);
            }
            else if (can_move)
            {
                unit_add_movement_animation(board->units, unit_index, board->selected_tile_index_x,
                                            board->selected_tile_index_y, move_x, move_y, REGULAR);
                // unit_occupy_new_tile(board->units, unit_index, board->selected_tile_index_x,
                // board->selected_tile_index_y, move_x, move_y, board->board_dimension_x);
                unit_cancel_action(board->units, unit_index);
                /*
                                unit_move(board->units, unit_index, board->selected_tile_index_x,
                   board->selected_tile_index_y, move_x, move_y, board->board_dimension_x, board->board_dimension_y);
                */
            }
        }
        board->selected_point = -1;
        board->last_selected_tile_index_x = board->selected_tile_index_x;
        board->last_selected_tile_index_y = board->selected_tile_index_y;
        board->selected_tile_index_x = -1;
        board->selected_tile_index_y = -1;
        da_int_clear(board->board_moveable_tiles);
        da_int_clear(board->board_attackable_tiles);
        da_int_clear(board->board_swapable_tiles);
        memset(board->board_fill_positions, 0, 12 * board->board_dimension_x * board->board_dimension_y * 2);
        memset(board->board_fill_colors, 0, 12 * board->board_dimension_x * board->board_dimension_y * 4);

        board->board_update_flags |= BOARD_UPDATE_FILL;
    }
}

void board_update_hovered_tile(Board *board, float mouse_board_pos_x, float mouse_board_pos_y)
{
    board->board_update_flags |= BOARD_UPDATE_FILL;

    float board_width =
        (float)board->board_dimension_x * board_get_hex_tile_width() * 0.75f + board_get_hex_tile_width() / 4.0f;
    float board_height =
        (float)board->board_dimension_y * board_get_hex_tile_height() + board_get_hex_tile_height() / 2.0f;
    if (mouse_board_pos_x < 0 || mouse_board_pos_x > board_width || mouse_board_pos_y < 0 ||
        mouse_board_pos_y > board_height)
    {
        board->mouse_tile_index_x = -1;
        board->mouse_tile_index_y = -1;
        board->hovered_point = -1;
        return;
    }
    board->mouse_tile_index_x = (int)floorf(mouse_board_pos_x / (3.0f * board_get_hex_tile_width() / 4.0f));
    board->mouse_tile_index_y =
        (int)(floorf((mouse_board_pos_y + board_get_hex_tile_height() / 2.0f * (float)(board->mouse_tile_index_x % 2)) /
                     board_get_hex_tile_height())) -
        board->mouse_tile_index_x % 2;
    float opposite =
        mouse_board_pos_y - ((float)board->mouse_tile_index_y * board_get_hex_tile_height() +
                             board_get_hex_tile_height() / ((float)((board->mouse_tile_index_x + 1) % 2) + 1.0f));
    float adjacent = mouse_board_pos_x - (float)board->mouse_tile_index_x * 3.0f * board_get_hex_tile_width() / 4.0f;
    float angle = atan2f(opposite, adjacent);
    if (angle > atan2f(board_get_hex_tile_height() / 2.0f, board_get_hex_tile_width() / 4.0f))
    {
        if (board->mouse_tile_index_x % 2 == 1)
        {
            board->mouse_tile_index_y++;
        }
        board->mouse_tile_index_x--;
    }
    else if (angle < -atan2f(board_get_hex_tile_height() / 2.0f, board_get_hex_tile_width() / 4.0f))
    {
        if (board->mouse_tile_index_x % 2 == 0)
        {
            board->mouse_tile_index_y--;
        }
        board->mouse_tile_index_x--;
    }

    board->hovered_point = coords_to_point(board->mouse_tile_index_x, board->mouse_tile_index_y,
                                           board->board_dimension_x, board->board_dimension_y);
    if (board->hovered_point == -1)
    {
        board->mouse_tile_index_x = -1;
        board->mouse_tile_index_y = -1;
    }
}

void board_add_fill_vertices(const float *board_point_vertices, float *board_fill_vertices, int board_vertices_index,
                             int start_point, int dimension_x, int dimension_y)
{
    if (start_point < 0 || start_point > dimension_y * (dimension_x * 2 + 2) + dimension_x * 2 + 2)
    {
        return;
    }
    int offset = dimension_x * 2 + 2;
    offset *= start_point % 4 == 0 ? 2 : -2;
    start_point = start_point * 2;
    board_fill_vertices[board_vertices_index] = board_point_vertices[start_point];
    board_fill_vertices[board_vertices_index + 1] = board_point_vertices[start_point + 1];
    board_fill_vertices[board_vertices_index + 2] = board_point_vertices[start_point + 2];
    board_fill_vertices[board_vertices_index + 3] = board_point_vertices[start_point + 3];
    board_fill_vertices[board_vertices_index + 4] = board_point_vertices[start_point + 4];
    board_fill_vertices[board_vertices_index + 5] = board_point_vertices[start_point + 5];

    board_fill_vertices[board_vertices_index + 6] = board_point_vertices[start_point];
    board_fill_vertices[board_vertices_index + 7] = board_point_vertices[start_point + 1];
    board_fill_vertices[board_vertices_index + 8] = board_point_vertices[start_point + 4];
    board_fill_vertices[board_vertices_index + 9] = board_point_vertices[start_point + 5];
    board_fill_vertices[board_vertices_index + 10] = board_point_vertices[start_point + 6];
    board_fill_vertices[board_vertices_index + 11] = board_point_vertices[start_point + 7];

    board_fill_vertices[board_vertices_index + 12] = board_point_vertices[start_point];
    board_fill_vertices[board_vertices_index + 13] = board_point_vertices[start_point + 1];
    board_fill_vertices[board_vertices_index + 14] = board_point_vertices[start_point + 6];
    board_fill_vertices[board_vertices_index + 15] = board_point_vertices[start_point + 7];
    board_fill_vertices[board_vertices_index + 16] = board_point_vertices[start_point + offset + 4];
    board_fill_vertices[board_vertices_index + 17] = board_point_vertices[start_point + offset + 5];

    board_fill_vertices[board_vertices_index + 18] = board_point_vertices[start_point];
    board_fill_vertices[board_vertices_index + 19] = board_point_vertices[start_point + 1];
    board_fill_vertices[board_vertices_index + 20] = board_point_vertices[start_point + offset + 4];
    board_fill_vertices[board_vertices_index + 21] = board_point_vertices[start_point + offset + 5];
    board_fill_vertices[board_vertices_index + 22] = board_point_vertices[start_point + offset + 2];
    board_fill_vertices[board_vertices_index + 23] = board_point_vertices[start_point + offset + 3];
}

void board_add_fill_colors(float *board_fill_colors, int board_colors_index, float r, float g, float b, float a)
{
    // 12 total vertices to a hex
    for (int i = 0; i < 12; i++)
    {
        board_fill_colors[board_colors_index + i * 4] = r;
        board_fill_colors[board_colors_index + i * 4 + 1] = g;
        board_fill_colors[board_colors_index + i * 4 + 2] = b;
        board_fill_colors[board_colors_index + i * 4 + 3] = a;
    }
}

void board_clear_fill_vertices(Board *board)
{
    memset(board->board_fill_positions, 0,
           sizeof(float) * 12 * board->board_dimension_x * board->board_dimension_y * 2);
    memset(board->board_fill_colors, 0, sizeof(float) * 12 * board->board_dimension_x * board->board_dimension_y * 4);
}

void board_update_hovered_tile_fill_vertices(Board *board)
{
    if (board->mouse_tile_index_x >= 0 && board->mouse_tile_index_x < board->board_dimension_x &&
        board->mouse_tile_index_y >= 0 && board->mouse_tile_index_y < board->board_dimension_y)
    {
        int hovered_tile_index =
            (board->mouse_tile_index_y * board->board_dimension_x + board->mouse_tile_index_x) * 12;
        board_add_fill_vertices(board->board_outline_vertices, board->board_fill_positions, hovered_tile_index * 2,
                                coords_to_point(board->mouse_tile_index_x, board->mouse_tile_index_y,
                                                board->board_dimension_x, board->board_dimension_y),
                                board->board_dimension_x, board->board_dimension_y);
        board_add_fill_colors(board->board_fill_colors, hovered_tile_index * 4, hovered_tile_color[0],
                              hovered_tile_color[1], hovered_tile_color[2], hovered_tile_color[3]);
    }
}

void board_update_moveable_unit_fill_vertices(Board *board)
{
    for (int i = 0; i < board->board_dimension_x * board->board_dimension_y; i++)
    {
        int unit_index = board->units->unit_tile_occupation_status[i];
        if (unit_index != -1)
        {
            if (board->units->unit_movement_points[unit_index] > 0.0f &&
                board->units->unit_owner[unit_index] == board->board_current_turn % 4 + 1)
            {
                int x = i % board->board_dimension_x;
                int y = i / board->board_dimension_x;
                board_add_fill_vertices(board->board_outline_vertices, board->board_fill_positions, i * 12 * 2,
                                        coords_to_point(x, y, board->board_dimension_x, board->board_dimension_y),
                                        board->board_dimension_x, board->board_dimension_y);
                board_add_fill_colors(board->board_fill_colors, i * 12 * 4, highlighted_tile_ready_color[0],
                                      highlighted_tile_ready_color[1], highlighted_tile_ready_color[2],
                                      highlighted_tile_ready_color[3]);
            }
        }
    }
}

void board_update_moveable_tile_fill_vertices(Board *board)
{
    float mod_r, mod_g, mod_b, mod_a;
    for (int i = 0; i < board->board_moveable_tiles->used; i += 2)
    {
        int x = board->board_moveable_tiles->array[i];
        int y = board->board_moveable_tiles->array[i + 1];
        int highlighted_tile_index = (y * board->board_dimension_x + x) * 12;
        board_add_fill_vertices(board->board_outline_vertices, board->board_fill_positions, highlighted_tile_index * 2,
                                coords_to_point(x, y, board->board_dimension_x, board->board_dimension_y),
                                board->board_dimension_x, board->board_dimension_y);
        mod_r = 1.0f;
        mod_g = 1.0f;
        mod_b = 1.0f;
        mod_a = highlighted_tile_color[3];
        if (board->mouse_tile_index_x == x && board->mouse_tile_index_y == y)
        {
            mod_r = hovered_tile_color[0];
            mod_g = hovered_tile_color[1];
            mod_b = hovered_tile_color[2];
            mod_a = hovered_tile_color[3];
        }
        board_add_fill_colors(board->board_fill_colors, highlighted_tile_index * 4, highlighted_tile_color[0] * mod_r,
                              highlighted_tile_color[1] * mod_g, highlighted_tile_color[2] * mod_b, mod_a);
    }
}

void board_update_attackable_tile_fill_vertices(Board *board)
{
    float mod_r;
    float mod_g;
    float mod_b;
    float mod_a;
    for (int i = 0; i < board->board_attackable_tiles->used; i += 2)
    {
        int x = board->board_attackable_tiles->array[i];
        int y = board->board_attackable_tiles->array[i + 1];
        int highlighted_tile_index = (y * board->board_dimension_x + x) * 12;
        board_add_fill_vertices(board->board_outline_vertices, board->board_fill_positions, highlighted_tile_index * 2,
                                coords_to_point(x, y, board->board_dimension_x, board->board_dimension_y),
                                board->board_dimension_x, board->board_dimension_y);
        mod_r = 1.0f;
        mod_g = 1.0f;
        mod_b = 1.0f;
        mod_a = highlighted_tile_color[3];
        if (board->mouse_tile_index_x == x && board->mouse_tile_index_y == y)
        {
            mod_r = hovered_tile_color[0];
            mod_g = hovered_tile_color[1];
            mod_b = hovered_tile_color[2];
            mod_a = hovered_tile_color[3];
        }
        board_add_fill_colors(board->board_fill_colors, highlighted_tile_index * 4,
                              highlighted_tile_occupied_color[0] * mod_r, highlighted_tile_occupied_color[1] * mod_g,
                              highlighted_tile_occupied_color[2] * mod_b, highlighted_tile_occupied_color[3]);
    }
}

void board_update_swapable_tile_fill_vertices(Board *board)
{
    float mod_r;
    float mod_g;
    float mod_b;
    float mod_a;
    for (int i = 0; i < board->board_swapable_tiles->used; i += 2)
    {
        int x = board->board_swapable_tiles->array[i];
        int y = board->board_swapable_tiles->array[i + 1];
        int highlighted_tile_index = (y * board->board_dimension_x + x) * 12;
        board_add_fill_vertices(board->board_outline_vertices, board->board_fill_positions, highlighted_tile_index * 2,
                                coords_to_point(x, y, board->board_dimension_x, board->board_dimension_y),
                                board->board_dimension_x, board->board_dimension_y);
        mod_r = 1.0f;
        mod_g = 1.0f;
        mod_b = 1.0f;
        mod_a = highlighted_tile_color[3];
        if (board->mouse_tile_index_x == x && board->mouse_tile_index_y == y)
        {
            mod_r = hovered_tile_color[0];
            mod_g = hovered_tile_color[1];
            mod_b = hovered_tile_color[2];
            mod_a = hovered_tile_color[3];
        }
        board_add_fill_colors(board->board_fill_colors, highlighted_tile_index * 4,
                              highlighted_tile_friendly_color[0] * mod_r, highlighted_tile_friendly_color[1] * mod_g,
                              highlighted_tile_friendly_color[2] * mod_b, highlighted_tile_friendly_color[3]);
    }
}

void board_update_selected_tile_fill_vertices(Board *board)
{
    if (board->selected_tile_index_x < 0 || board->selected_tile_index_x >= board->board_dimension_x ||
        board->selected_tile_index_y < 0 || board->selected_tile_index_y >= board->board_dimension_y)
    {
        return;
    }
    float mod_r, mod_g, mod_b, mod_a;
    mod_r = 1.0f;
    mod_g = 1.0f;
    mod_b = 1.0f;
    mod_a = selected_tile_color[3];
    int selected_tile_index =
        (board->selected_tile_index_y * board->board_dimension_x + board->selected_tile_index_x) * 12;
    board_add_fill_vertices(board->board_outline_vertices, board->board_fill_positions, selected_tile_index * 2,
                            coords_to_point(board->selected_tile_index_x, board->selected_tile_index_y,
                                            board->board_dimension_x, board->board_dimension_y),
                            board->board_dimension_x, board->board_dimension_y);
    if (board->mouse_tile_index_x == board->selected_tile_index_x &&
        board->mouse_tile_index_y == board->selected_tile_index_y)
    {
        mod_r = hovered_tile_color[0];
        mod_g = hovered_tile_color[1];
        mod_b = hovered_tile_color[2];
        mod_a = hovered_tile_color[3];
    }
    board_add_fill_colors(board->board_fill_colors, selected_tile_index * 4, selected_tile_color[0] * mod_r,
                          selected_tile_color[1] * mod_g, selected_tile_color[2] * mod_b, mod_a);
}

void board_update_highlighted_path_fill_vertices(Board *board)
{
    if (board->board_highlighted_path != NULL)
    {
        for (int i = 0; i < board->board_highlighted_path->used; i += 2)
        {
            int x = board->board_highlighted_path->array[i];
            int y = board->board_highlighted_path->array[i + 1];
            int highlighted_tile_index = (y * board->board_dimension_x + x) * 12;
            board_add_fill_vertices(board->board_outline_vertices, board->board_fill_positions,
                                    highlighted_tile_index * 2,
                                    coords_to_point(x, y, board->board_dimension_x, board->board_dimension_y),
                                    board->board_dimension_x, board->board_dimension_y);
            board_add_fill_colors(board->board_fill_colors, highlighted_tile_index * 4, highlighted_tile_path_color[0],
                                  highlighted_tile_path_color[1], highlighted_tile_path_color[2],
                                  highlighted_tile_path_color[3]);
        }
    }
}

void board_update_border(Board *board)
{
    int dimension_x = board->board_dimension_x;
    int dimension_y = board->board_dimension_y;
    memset(board->board_border_positions, 0, sizeof(float) * 12 * board->board_borders_count);
    memset(board->board_border_uvs, 0, sizeof(float) * 12 * board->board_borders_count);
    memset(board->board_border_colors, 0, sizeof(float) * 24 * board->board_borders_count);
    int index = 0;
    for (int i = 0; i < dimension_x * dimension_y; i++)
    {
        if (board->units->unit_tile_ownership_status[i] == 0)
        {
            continue;
        }
        int x = i % dimension_x;
        int y = i / dimension_x;
        int bottom = i + dimension_x;
        int bottom_left = i + (x % 2 == 0 ? -1 : dimension_x - 1);
        int bottom_right = i + (x % 2 == 0 ? 1 : dimension_x + 1);
        int top = i - dimension_x;
        int top_left = i - (x % 2 == 0 ? dimension_x + 1 : 1);
        int top_right = i - (x % 2 == 0 ? dimension_x - 1 : -1);
        if (bottom >= dimension_x * dimension_y ||
            board->units->unit_tile_ownership_status[bottom] != board->units->unit_tile_ownership_status[i])
        {
            board_set_tile_border_vertices(board, x, y, 0, index);
            index += 12;
        }
        if (x == 0 || bottom_left < 0 || bottom_left >= dimension_x * dimension_y ||
            board->units->unit_tile_ownership_status[bottom_left] != board->units->unit_tile_ownership_status[i])
        {
            board_set_tile_border_vertices(board, x, y, 1, index);
            index += 12;
        }
        if (x == dimension_x - 1 || bottom_right < 0 || bottom_right >= dimension_x * dimension_y ||
            board->units->unit_tile_ownership_status[bottom_right] != board->units->unit_tile_ownership_status[i])
        {
            board_set_tile_border_vertices(board, x, y, 2, index);
            index += 12;
        }
        if (top < 0 || board->units->unit_tile_ownership_status[top] != board->units->unit_tile_ownership_status[i])
        {
            board_set_tile_border_vertices(board, x, y, 3, index);
            index += 12;
        }
        if (x == 0 || top_left < 0 || top_left >= dimension_x * dimension_y ||
            board->units->unit_tile_ownership_status[top_left] != board->units->unit_tile_ownership_status[i])
        {
            board_set_tile_border_vertices(board, x, y, 4, index);
            index += 12;
        }
        if (x == dimension_x - 1 || top_right < 0 || top_right >= dimension_x * dimension_y ||
            board->units->unit_tile_ownership_status[top_right] != board->units->unit_tile_ownership_status[i])
        {
            board_set_tile_border_vertices(board, x, y, 5, index);
            index += 12;
        }
    }
    board->board_borders_count = index / 12;
}

void board_process_turn(Board *board)
{
    // TODO: maybe do this in a function
    if (board->board_moveable_tiles != NULL)
    {
        da_int_clear(board->board_moveable_tiles);
    }
    da_int_clear(board->board_attackable_tiles);
    board->selected_tile_index_x = -1;
    board->selected_tile_index_y = -1;
    board->selected_point = -1;

    board->board_current_turn++;
}

void board_process_planet_orbit(Board *board)
{
    if (board->board_current_turn % board->player_count == 0)
    {
        DynamicIntArray moved_units;
        da_int_init(&moved_units, 2);
        for (int i = board->planets->planet_buffer_size - 1; i > 0; i--)
        {
            int x = board->planets->planet_tile_indices[i * 2];
            int y = board->planets->planet_tile_indices[i * 2 + 1];
            int unit_index = board->units->unit_tile_occupation_status[y * board->board_dimension_x + x];
            if (unit_index != -1)
            {
                bool unit_already_moved = false;
                for (int j = 0; j < moved_units.used; j++)
                {
                    if (unit_index == moved_units.array[j])
                    {
                        unit_already_moved = true;
                    }
                }
                if (unit_already_moved)
                {
                    continue;
                }
                int relative_x, relative_y;
                relative_x = x - board->planets->planet_tile_indices[0];
                relative_y = y - board->planets->planet_tile_indices[1];
                int q, r;
                hex_grid_offset_to_axial(relative_x, relative_y, board->planets->planet_tile_indices[1], &q, &r);
                hex_grid_rotation_get_next(false, board->planets->planet_distance_from_sun[i], &q, &r);
                hex_grid_axial_to_offset(q, r, board->planets->planet_tile_indices[1], &relative_x, &relative_y);
                int new_x = board->planets->planet_tile_indices[0] + relative_x;
                int new_y = board->planets->planet_tile_indices[1] + relative_y;
                int next_tile_unit_index =
                    board->units->unit_tile_occupation_status[new_y * board->board_dimension_x + new_x];
                unit_swap(board->units, unit_index, next_tile_unit_index, x, y, new_x, new_y, board->board_dimension_x,
                          false);
                da_int_push_back(&moved_units, unit_index);
            }
        }
        planet_orbit(board->planets);
        da_int_free(&moved_units);
    }
}

BattleResult board_process_attack(Board *board, int defender_index, int attacker_index)
{
    int move_x = board->units->unit_indices[defender_index * 2];
    int move_y = board->units->unit_indices[defender_index * 2 + 1];
    switch (unit_attack(board->units, defender_index, attacker_index))
    {
    case NO_UNITS_DESTROYED: {
        return NO_UNITS_DESTROYED;
    }
    case DEFENDER_CAPTURED:
    case DEFENDER_DESTROYED: {
        if (board->units->unit_type[defender_index] != STATION)
        {
            unit_remove(board->units, defender_index, move_x, move_y, board->board_dimension_x);
            return DEFENDER_DESTROYED;
        }
        board->units->unit_owner[defender_index] = board->units->unit_owner[attacker_index];
        board->units->unit_health[defender_index] = 0.0f;
        unit_update_color(board->units, defender_index);
        unit_claim_territory(board->units, defender_index, move_x, move_y, board->board_dimension_x,
                             board->board_dimension_y);
        board->units->unit_update_flags |= UNIT_UPDATE_HEALTH;
        board->board_update_flags |= BOARD_UPDATE_BORDERS;
        return DEFENDER_CAPTURED;
    }
    case ATTACKER_DESTROYED: {
        unit_remove(board->units, attacker_index, board->units->unit_indices[attacker_index * 2],
                    board->units->unit_indices[attacker_index * 2 + 1], board->board_dimension_x);
        return ATTACKER_DESTROYED;
    }
    case BOTH_DESTROYED: {
        unit_remove(board->units, defender_index, move_x, move_y, board->board_dimension_x);
        unit_remove(board->units, attacker_index, board->units->unit_indices[attacker_index * 2],
                    board->units->unit_indices[attacker_index * 2 + 1], board->board_dimension_x);
        return BOTH_DESTROYED;
    }
    }
    return -1;
}

bool board_tile_in_bounds(Board *board, int x, int y)
{
    return x > -1 && x < board->board_dimension_x && y > -1 && y < board->board_dimension_y;
}

bool board_tile_is_occupied(Board *board, int x, int y)
{
    return (x < 0 || x >= board->board_dimension_x || y < 0 || y >= board->board_dimension_y ||
            board->units->unit_tile_occupation_status[y * board->board_dimension_x + x] != -1 ||
            (x == board->planets->planet_tile_indices[0] && y == board->planets->planet_tile_indices[1]));
}

void board_highlight_possible_unit_placement(Board *board, int station_index, int station_x, int station_y)
{
    da_int_free(board->board_moveable_tiles);
    free(board->board_moveable_tiles);
    board->board_moveable_tiles = hex_grid_possible_moves(board, station_index, station_x, station_y);
    board->board_update_flags |= BOARD_UPDATE_FILL;
}

void board_worker_build_station(Board *board, int worker_index)
{
    da_int_push_back(&board->units->current_status_unit_index, worker_index);
    da_int_push_back(&board->units->unit_current_status, UNIT_BUILD_STATION);
    da_int_push_back(&board->units->unit_status_started, board->board_current_turn / board->player_count);
}

int board_get_hex_tile_width()
{
    return BOARD_HEX_TILE_WIDTH * ((float)camera_get_viewport_width() / 1280.0f);
}

int board_get_hex_tile_height()
{
    return BOARD_HEX_TILE_HEIGHT * ((float)camera_get_viewport_height() / 720.0f);
}

void board_clear(Board *board)
{
    board->board_dimension_x = 0;
    board->board_dimension_y = 0;
    board->player_count = 0;
    board->hovered_point = -1;
    board->selected_point = -1;
    board->mouse_tile_index_x = -1;
    board->mouse_tile_index_y = -1;
    board->selected_tile_index_x = -1;
    board->selected_tile_index_y = -1;
    board->board_borders_count = 0;
    free(board->board_fill_positions);
    board->board_fill_positions = NULL;
    free(board->board_fill_colors);
    board->board_fill_colors = NULL;
    free(board->board_outline_vertices);
    board->board_outline_vertices = NULL;
    free(board->board_outline_indices);
    board->board_outline_indices = NULL;
    free(board->board_border_positions);
    board->board_border_positions = NULL;
    free(board->board_border_uvs);
    board->board_border_uvs = NULL;
    free(board->board_border_colors);
    board->board_border_colors = NULL;
    da_int_free(board->board_highlighted_path);
    free(board->board_highlighted_path);
    board->board_highlighted_path = NULL;
    da_int_free(board->board_moveable_tiles);
    free(board->board_moveable_tiles);
    board->board_moveable_tiles = NULL;
    da_int_free(board->board_attackable_tiles);
    free(board->board_attackable_tiles);
    board->board_attackable_tiles = NULL;
    da_int_free(board->board_swapable_tiles);
    free(board->board_swapable_tiles);
    board->board_swapable_tiles = NULL;
    units_destroy(board->units);
    board->units = NULL;
    planets_destroy(board->planets);
    board->planets = NULL;
}

void board_destroy(Board *board)
{
    board_clear(board);
    free(board);
}