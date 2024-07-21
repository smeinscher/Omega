//
// Created by sterling on 6/26/24.
//

#include "board.h"

#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

static float hovered_tile_color[4] = {0.7f, 0.7f, 0.7f, 0.6f};
static float selected_tile_color[4] = {0.2f, 0.7f, 0.9f, 0.3f};
static float highlighted_tile_color[4] = {0.2f, 0.7f, 0.9f, 0.2f};

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
            (float)board_offset_y + (float)i * BOARD_HEX_TILE_HEIGHT + BOARD_HEX_TILE_HEIGHT / 2.0f;
        for (int j = 0; j < dimension_x; j++)
        {
            const float y_offset = (float)(j % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f;
            board_vertices[index++] =
                (float)board_offset_x + (float)j * 3.0f * BOARD_HEX_TILE_WIDTH / 4.0f + BOARD_HEX_TILE_WIDTH / 4.0f;
            board_vertices[index++] = (float)board_offset_y + (float)i * BOARD_HEX_TILE_HEIGHT + y_offset;
            board_vertices[index++] = (float)board_offset_x + (float)j * 3.0f * BOARD_HEX_TILE_WIDTH / 4.0f +
                                      3.0f * BOARD_HEX_TILE_WIDTH / 4.0f;
            board_vertices[index++] = (float)board_offset_y + (float)i * BOARD_HEX_TILE_HEIGHT + y_offset;
        }
        board_vertices[index++] = (float)board_offset_x + (float)dimension_x * 3.0f * BOARD_HEX_TILE_WIDTH / 4.0f +
                                  BOARD_HEX_TILE_WIDTH / 4.0f;
        board_vertices[index++] = (float)board_offset_y + (float)i * BOARD_HEX_TILE_HEIGHT +
                                  BOARD_HEX_TILE_HEIGHT / (1.0f + (float)(dimension_x % 2));
    }
    board_vertices[index++] = (float)board_offset_x;
    board_vertices[index++] =
        (float)board_offset_y + (float)dimension_y * BOARD_HEX_TILE_HEIGHT + BOARD_HEX_TILE_HEIGHT / 2.0f;
    for (int j = 0; j < dimension_x; j++)
    {
        const float y_offset = (float)(j % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f;
        board_vertices[index++] =
            (float)board_offset_x + (float)(j) * 3.0f * BOARD_HEX_TILE_WIDTH / 4.0f + BOARD_HEX_TILE_WIDTH / 4.0f;
        board_vertices[index++] = (float)board_offset_y + (float)dimension_y * BOARD_HEX_TILE_HEIGHT + y_offset;
        board_vertices[index++] =
            (float)board_offset_x + (float)j * 3.0f * BOARD_HEX_TILE_WIDTH / 4.0f + 3.0f * BOARD_HEX_TILE_WIDTH / 4.0f;
        board_vertices[index++] = (float)board_offset_y + (float)dimension_y * BOARD_HEX_TILE_HEIGHT + y_offset;
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

    board->board_border_positions[start_index] = (float)x * BOARD_HEX_TILE_WIDTH * 0.75f;
    board->board_border_positions[start_index + 1] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f + BOARD_HEX_TILE_HEIGHT;
    board->board_border_positions[start_index + 2] = (float)x * BOARD_HEX_TILE_WIDTH + BOARD_HEX_TILE_WIDTH;
    board->board_border_positions[start_index + 3] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f;
    board->board_border_positions[start_index + 4] = (float)x * BOARD_HEX_TILE_WIDTH * 0.75f;
    board->board_border_positions[start_index + 5] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f;
    board->board_border_positions[start_index + 6] = (float)x * BOARD_HEX_TILE_WIDTH * 0.75f;
    board->board_border_positions[start_index + 7] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f + BOARD_HEX_TILE_HEIGHT;
    board->board_border_positions[start_index + 8] = (float)x * BOARD_HEX_TILE_WIDTH + BOARD_HEX_TILE_WIDTH;
    board->board_border_positions[start_index + 9] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f;
    board->board_border_positions[start_index + 10] = (float)x * BOARD_HEX_TILE_WIDTH + BOARD_HEX_TILE_WIDTH;
    board->board_border_positions[start_index + 11] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f + BOARD_HEX_TILE_HEIGHT;

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
}

void board_set_tile_ownership(Board *board)
{
    memset(board->tile_ownership_status, 0, sizeof(int) * board->board_dimension_x * board->board_dimension_y);
    board->tile_ownership_status[0] = 1;
    board->board_borders_count = 2;
}

// TODO: Error checking for malloc
Board *board_create(int dimension_x, int dimension_y)
{
    Board *board;
    board = malloc(sizeof(Board));
    if (!board)
    {
        printf("Error allocating board\n");
        return NULL;
    }
    board->board_dimension_x = dimension_x;
    board->board_dimension_y = dimension_y;
    board->hovered_point = -1;
    board->selected_point = -1;
    board->mouse_tile_index_x = -1;
    board->mouse_tile_index_y = -1;
    board->selected_tile_index_x = -1;
    board->selected_tile_index_y = -1;
    board->board_borders_count = -1;
    board->selected_tiles = NULL;
    board->board_fill_positions = NULL;
    board->board_fill_colors = NULL;
    board->tile_occupation_status = NULL;
    board->tile_ownership_status = NULL;
    board->board_outline_vertices = NULL;
    board->board_outline_indices = NULL;
    board->board_border_positions = NULL;
    board->board_border_uvs = NULL;
    board->board_border_colors = NULL;

    board->tile_occupation_status = malloc(sizeof(int) * dimension_x * dimension_y);
    if (board->tile_occupation_status == NULL)
    {
        printf("Error allocating tile_occupation_status\n");
        board_destroy(board);
        return NULL;
    }
    memset(board->tile_occupation_status, 0, sizeof(int) * dimension_x * dimension_y);
    board->tile_ownership_status = malloc(sizeof(int) * dimension_x * dimension_y);
    if (board->tile_ownership_status == NULL)
    {
        printf("Error allocating tile_ownership_status\n");
        board_destroy(board);
        return NULL;
    }
    board_set_tile_ownership(board);

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

    board->board_border_positions = malloc(sizeof(float) * 12 * board->board_borders_count);
    if (board->board_border_positions == NULL)
    {
        printf("Error allocating board_border_positions\n");
        board_destroy(board);
        return NULL;
    }
    memset(board->board_border_positions, 0, sizeof(float) * 12 * board->board_borders_count);
    board->board_border_uvs = malloc(sizeof(float) * 12 * board->board_borders_count);
    if (board->board_border_uvs == NULL)
    {
        printf("Error allocating board_border_uvs\n");
        board_destroy(board);
        return NULL;
    }
    memset(board->board_border_uvs, 0, sizeof(float) * 12 * board->board_borders_count);
    int index = 0;
    for (int i = 0; i < dimension_x * dimension_y; i++)
    {
        if (board->tile_ownership_status[i] != 0)
        {
            int x = i % dimension_x;
            int y = i / dimension_x;
            int bottom = i + dimension_x;
            int bottom_left = i + (i % 2 == 0 ? -1 : dimension_x - 1);
            int bottom_right = i + (i % 2 == 0 ? 1 : dimension_x + 1);
            int top = i - dimension_x;
            int top_left = i - (i % 2 == 0 ? dimension_x + 1 : 1);
            int top_right = i - (i % 2 == 0 ? dimension_x - 1 : -1);
            if (bottom > 0 && bottom < dimension_x * dimension_y &&
                board->tile_ownership_status[bottom] != board->tile_ownership_status[i])
            {
                board_set_tile_border_vertices(board, x, y, 0, index);
                index += 12;
            }
            if (bottom_left > 0 && bottom_left < dimension_x * dimension_y &&
                board->tile_ownership_status[bottom_left] != board->tile_ownership_status[i])
            {
                board_set_tile_border_vertices(board, x, y, 1, index);
                index += 12;
            }
            if (bottom_right > 0 && bottom_right < dimension_x * dimension_y &&
                board->tile_ownership_status[bottom_right] != board->tile_ownership_status[i])
            {
                board_set_tile_border_vertices(board, x, y, 2, index);
                index += 12;
            }
            if (top > 0 && top < dimension_x * dimension_y &&
                board->tile_ownership_status[top] != board->tile_ownership_status[i])
            {
                board_set_tile_border_vertices(board, x, y, 3, index);
                index += 12;
            }
            if (top_left > 0 && top_left < dimension_x * dimension_y &&
                board->tile_ownership_status[top_left] != board->tile_ownership_status[i])
            {
                board_set_tile_border_vertices(board, x, y, 4, index);
                index += 12;
            }
            if (top_right > 0 && top_right < dimension_x * dimension_y &&
                board->tile_ownership_status[top_right] != board->tile_ownership_status[i])
            {
                board_set_tile_border_vertices(board, x, y, 5, index);
                index += 12;
            }
        }
    }

    board->board_border_colors = malloc(sizeof(float) * 24 * board->board_borders_count);
    if (board->board_border_colors == NULL)
    {
        printf("Error allocating board_border_colors\n");
        board_destroy(board);
        return NULL;
    }
    for (int i = 0; i < 6 * board->board_borders_count; i++)
    {
        board->board_border_colors[i * 4] = 0.7f;
        board->board_border_colors[i * 4 + 1] = 0.0f;
        board->board_border_colors[i * 4 + 2] = 0.0f;
        board->board_border_colors[i * 4 + 3] = 1.0f;
    }

    // Allocate memory for all tiles on board
    // TODO: do this a smarter way
    board->selected_tiles = malloc(sizeof(int) * dimension_x * dimension_y * 2);
    if (board->selected_tiles == NULL)
    {
        printf("Error allocating selected_tiles\n");
        board_destroy(board);
        return NULL;
    }
    memset(board->selected_tiles, -1, sizeof(int) * dimension_x * dimension_y * 2);
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
        printf("Error allocating board_fill_colors");
        board_destroy(board);
        return NULL;
    }
    memset(board->board_fill_colors, 0, sizeof(float) * num_vertices * 4);
    return board;
}

void board_handle_tile_click(Board *board)
{
    if (board->selected_point == -1)
    {
        if (board->hovered_point == -1)
        {
            return;
        }
        board->selected_point = board->hovered_point;
        board->selected_tile_index_x = board->mouse_tile_index_x;
        board->selected_tile_index_y = board->mouse_tile_index_y;
        if (board->tile_occupation_status[board->selected_tile_index_y * board->board_dimension_x +
                                          board->selected_tile_index_x] != 1)
        {
            return;
        }
        // Center
        board->selected_tiles[0] = board->mouse_tile_index_x;
        board->selected_tiles[1] = board->mouse_tile_index_y - 1;
        board->selected_tiles[2] = board->mouse_tile_index_x;
        board->selected_tiles[3] = board->mouse_tile_index_y - 2;
        board->selected_tiles[4] = board->mouse_tile_index_x;
        board->selected_tiles[5] = board->mouse_tile_index_y + 1;
        board->selected_tiles[6] = board->mouse_tile_index_x;
        board->selected_tiles[7] = board->mouse_tile_index_y + 2;
        // End
        board->selected_tiles[8] = board->mouse_tile_index_x - 2;
        board->selected_tiles[9] = board->mouse_tile_index_y;
        board->selected_tiles[10] = board->mouse_tile_index_x - 2;
        board->selected_tiles[11] = board->mouse_tile_index_y - 1;
        board->selected_tiles[12] = board->mouse_tile_index_x - 2;
        board->selected_tiles[13] = board->mouse_tile_index_y + 1;
        board->selected_tiles[14] = board->mouse_tile_index_x + 2;
        board->selected_tiles[15] = board->mouse_tile_index_y;
        board->selected_tiles[16] = board->mouse_tile_index_x + 2;
        board->selected_tiles[17] = board->mouse_tile_index_y - 1;
        board->selected_tiles[18] = board->mouse_tile_index_x + 2;
        board->selected_tiles[19] = board->mouse_tile_index_y + 1;
        // Middle
        board->selected_tiles[20] = board->mouse_tile_index_x - 1;
        board->selected_tiles[21] = board->mouse_tile_index_y;
        board->selected_tiles[22] = board->mouse_tile_index_x + 1;
        board->selected_tiles[23] = board->mouse_tile_index_y;
        if (board->mouse_tile_index_x % 2 == 0)
        {
            board->selected_tiles[24] = board->mouse_tile_index_x - 1;
            board->selected_tiles[25] = board->mouse_tile_index_y - 1;
            board->selected_tiles[26] = board->mouse_tile_index_x - 1;
            board->selected_tiles[27] = board->mouse_tile_index_y - 2;
            board->selected_tiles[28] = board->mouse_tile_index_x - 1;
            board->selected_tiles[29] = board->mouse_tile_index_y + 1;

            board->selected_tiles[30] = board->mouse_tile_index_x + 1;
            board->selected_tiles[31] = board->mouse_tile_index_y - 1;
            board->selected_tiles[32] = board->mouse_tile_index_x + 1;
            board->selected_tiles[33] = board->mouse_tile_index_y - 2;
            board->selected_tiles[34] = board->mouse_tile_index_x + 1;
            board->selected_tiles[35] = board->mouse_tile_index_y + 1;
        }
        else
        {
            board->selected_tiles[24] = board->mouse_tile_index_x - 1;
            board->selected_tiles[25] = board->mouse_tile_index_y - 1;
            board->selected_tiles[26] = board->mouse_tile_index_x - 1;
            board->selected_tiles[27] = board->mouse_tile_index_y + 1;
            board->selected_tiles[28] = board->mouse_tile_index_x - 1;
            board->selected_tiles[29] = board->mouse_tile_index_y + 2;

            board->selected_tiles[30] = board->mouse_tile_index_x + 1;
            board->selected_tiles[31] = board->mouse_tile_index_y - 1;
            board->selected_tiles[32] = board->mouse_tile_index_x + 1;
            board->selected_tiles[33] = board->mouse_tile_index_y + 1;
            board->selected_tiles[34] = board->mouse_tile_index_x + 1;
            board->selected_tiles[35] = board->mouse_tile_index_y + 2;
        }
    }
    else
    {
        if (board->tile_occupation_status[board->selected_tile_index_y * board->board_dimension_y +
                                          board->selected_tile_index_x] == 1)
        {
            board->tile_occupation_status[board->mouse_tile_index_y * board->board_dimension_x +
                                          board->mouse_tile_index_x] = 1;
            board->tile_occupation_status[board->selected_tile_index_y * board->board_dimension_y +
                                          board->selected_tile_index_x] = 0;
        }
        board->selected_point = -1;
        board->selected_tile_index_x = -1;
        board->selected_tile_index_y = -1;
        memset(board->selected_tiles, -1, board->board_dimension_x * board->board_dimension_y * 2);
        memset(board->board_fill_positions, 0, 12 * board->board_dimension_x * board->board_dimension_y * 2);
        memset(board->board_fill_colors, 0, 12 * board->board_dimension_x * board->board_dimension_y * 4);
    }
}

void board_update_hovered_tile(Board *board, float mouse_board_pos_x, float mouse_board_pos_y)
{
    float board_width = (float)board->board_dimension_x * BOARD_HEX_TILE_WIDTH * 0.75f + BOARD_HEX_TILE_WIDTH / 4.0f;
    float board_height = (float)board->board_dimension_y * BOARD_HEX_TILE_HEIGHT + BOARD_HEX_TILE_HEIGHT / 2.0f;
    if (mouse_board_pos_x < 0 || mouse_board_pos_x > board_width || mouse_board_pos_y < 0 ||
        mouse_board_pos_y > board_height)
    {
        board->mouse_tile_index_x = -1;
        board->mouse_tile_index_y = -1;
        board->hovered_point = -1;
        return;
    }
    board->mouse_tile_index_x = (int)floorf(mouse_board_pos_x / (3.0f * BOARD_HEX_TILE_WIDTH / 4.0f));
    board->mouse_tile_index_y =
        (int)(floorf((mouse_board_pos_y + BOARD_HEX_TILE_HEIGHT / 2.0f * (float)(board->mouse_tile_index_x % 2)) /
                     BOARD_HEX_TILE_HEIGHT)) -
        board->mouse_tile_index_x % 2;
    float opposite =
        mouse_board_pos_y - ((float)board->mouse_tile_index_y * BOARD_HEX_TILE_HEIGHT +
                             BOARD_HEX_TILE_HEIGHT / ((float)((board->mouse_tile_index_x + 1) % 2) + 1.0f));
    float adjacent = mouse_board_pos_x - (float)board->mouse_tile_index_x * 3.0f * BOARD_HEX_TILE_WIDTH / 4.0f;
    float angle = atan2f(opposite, adjacent);
    if (angle > atan2f(BOARD_HEX_TILE_HEIGHT / 2.0f, BOARD_HEX_TILE_WIDTH / 4.0f))
    {
        if (board->mouse_tile_index_x % 2 == 1)
        {
            board->mouse_tile_index_y++;
        }
        board->mouse_tile_index_x--;
    }
    else if (angle < -atan2f(BOARD_HEX_TILE_HEIGHT / 2.0f, BOARD_HEX_TILE_WIDTH / 4.0f))
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

void board_update_fill_vertices(Board *board)
{
    memset(board->board_fill_positions, 0,
           sizeof(float) * 12 * board->board_dimension_x * board->board_dimension_y * 2);
    memset(board->board_fill_colors, 0, sizeof(float) * 12 * board->board_dimension_x * board->board_dimension_y * 4);
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
    if (board->selected_tile_index_x < 0 || board->selected_tile_index_x >= board->board_dimension_x ||
        board->selected_tile_index_y < 0 || board->selected_tile_index_y >= board->board_dimension_y)
    {
        return;
    }
    int selected_tile_index =
        (board->selected_tile_index_y * board->board_dimension_x + board->selected_tile_index_x) * 12;
    board_add_fill_vertices(board->board_outline_vertices, board->board_fill_positions, selected_tile_index * 2,
                            coords_to_point(board->selected_tile_index_x, board->selected_tile_index_y,
                                            board->board_dimension_x, board->board_dimension_y),
                            board->board_dimension_x, board->board_dimension_y);
    float mod_r = 1.0f;
    float mod_g = 1.0f;
    float mod_b = 1.0f;
    float mod_a = selected_tile_color[3];
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
    for (int i = 0; i < 36; i += 2)
    {
        int x = board->selected_tiles[i];
        int y = board->selected_tiles[i + 1];
        if (x < 0 || x >= board->board_dimension_x || y < 0 || y >= board->board_dimension_y)
        {
            continue;
        }
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

void board_clear(Board *board)
{
    board->board_dimension_x = 0;
    board->board_dimension_y = 0;
    board->hovered_point = -1;
    board->selected_point = -1;
    board->mouse_tile_index_x = -1;
    board->mouse_tile_index_y = -1;
    board->selected_tile_index_x = -1;
    board->selected_tile_index_y = -1;
    free(board->selected_tiles);
    board->selected_tiles = NULL;
    free(board->board_fill_positions);
    board->board_fill_positions = NULL;
    free(board->board_fill_colors);
    board->board_fill_colors = NULL;
    free(board->tile_occupation_status);
    board->tile_occupation_status = NULL;
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
}

void board_destroy(Board *board)
{
    board_clear(board);
    free(board);
}
