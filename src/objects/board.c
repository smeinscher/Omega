//
// Created by sterling on 6/26/24.
//

#include "board.h"

#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

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

// TODO: Error checking for malloc
Board *board_create(int dimension_x, int dimension_y)
{
    Board *board;
    board = malloc(sizeof(Board));
    board->board_dimension_x = dimension_x;
    board->board_dimension_y = dimension_y;
    board->hovered_point = -1;
    board->selected_point = -1;
    board->mouse_tile_index_x = -1;
    board->mouse_tile_index_y = -1;
    board->selected_tile_index_x = -1;
    board->selected_tile_index_y = -1;
    board->tile_occupation_status = malloc(sizeof(int) * dimension_x * dimension_y);

    board->board_outline_vertices = board_vertices_create(dimension_x, dimension_y, 0, 0);
    board->board_outline_indices = board_outline_indices_create(dimension_x, dimension_y);

    board->board_border_positions = malloc(sizeof(float) * 12);
    board->board_border_positions[0] = 0.0f;
    board->board_border_positions[1] = BOARD_HEX_TILE_HEIGHT;
    board->board_border_positions[2] = BOARD_HEX_TILE_WIDTH;
    board->board_border_positions[3] = 0.0f;
    board->board_border_positions[4] = 0.0f;
    board->board_border_positions[5] = 0.0f;
    board->board_border_positions[6] = 0.0f;
    board->board_border_positions[7] = BOARD_HEX_TILE_HEIGHT;
    board->board_border_positions[8] = BOARD_HEX_TILE_WIDTH;
    board->board_border_positions[9] = 0.0f;
    board->board_border_positions[10] = BOARD_HEX_TILE_WIDTH;
    board->board_border_positions[11] = BOARD_HEX_TILE_HEIGHT;

    board->board_border_uvs = malloc(sizeof(float) * 12);
    board->board_border_uvs[0] = 0.0f;
    board->board_border_uvs[1] = 1.0f;
    board->board_border_uvs[2] = 1.0f;
    board->board_border_uvs[3] = 0.0f;
    board->board_border_uvs[4] = 0.0f;
    board->board_border_uvs[5] = 0.0f;
    board->board_border_uvs[6] = 0.0f;
    board->board_border_uvs[7] = 1.0f;
    board->board_border_uvs[8] = 1.0f;
    board->board_border_uvs[9] = 0.0f;
    board->board_border_uvs[10] = 1.0f;
    board->board_border_uvs[11] = 1.0f;

    board->board_border_colors = malloc(sizeof(float) * 24);
    for (int i = 0; i < 6; i++)
    {
        board->board_border_colors[i * 4] = 0.7f;
        board->board_border_colors[i * 4 + 1] = 0.7f;
        board->board_border_colors[i * 4 + 2] = 0.7f;
        board->board_border_colors[i * 4 + 3] = 1.0f;
    }

    board->board_tile_offsets = malloc(sizeof(float) * dimension_y * dimension_x * 2);
    if (board->board_tile_offsets == NULL)
    {
        // TODO: logging stuff
        printf("Error allocating board_tile_offsets\n");
    }
    for (int i = 0; i < dimension_y; i++)
    {
        for (int j = 0; j < dimension_x; j++)
        {
            board->board_tile_offsets[(i * dimension_x + j) * 2] = (float)j * BOARD_HEX_TILE_WIDTH * 0.75f;
            board->board_tile_offsets[(i * dimension_x + j) * 2 + 1] =
                (float)i * BOARD_HEX_TILE_HEIGHT + (float)(j % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f;
        }
    }

    board->selected_tiles = malloc(sizeof(int) * dimension_x * dimension_y);
    unsigned int num_vertices = dimension_y * dimension_x * 24;
    board->selected_tiles_vertices = malloc(sizeof(float) * num_vertices);
    memset(board->selected_tiles_vertices, 0, num_vertices);
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
        memset(board->selected_tiles, -1, 36);
        memset(board->selected_tiles_vertices, 0, 38 * 12);
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

void board_update_fill_vertices(Board *board)
{
    memset(board->selected_tiles_vertices, 0, 40 * 12);
    if (board->mouse_tile_index_x >= 0 && board->mouse_tile_index_x < board->board_dimension_x &&
        board->mouse_tile_index_y >= 0 && board->mouse_tile_index_y < board->board_dimension_y)
    {
        board_add_fill_vertices(board->board_outline_vertices, board->selected_tiles_vertices, 0,
                                coords_to_point(board->mouse_tile_index_x, board->mouse_tile_index_y,
                                                board->board_dimension_x, board->board_dimension_y),
                                board->board_dimension_x, board->board_dimension_y);
    }
    if (board->selected_tile_index_x < 0 || board->selected_tile_index_x >= board->board_dimension_x ||
        board->selected_tile_index_y < 0 || board->selected_tile_index_y >= board->board_dimension_y)
    {
        return;
    }
    board_add_fill_vertices(board->board_outline_vertices, board->selected_tiles_vertices, 24,
                            coords_to_point(board->selected_tile_index_x, board->selected_tile_index_y,
                                            board->board_dimension_x, board->board_dimension_y),
                            board->board_dimension_x, board->board_dimension_y);
    for (int i = 0; i < 36; i += 2)
    {
        int x = board->selected_tiles[i];
        int y = board->selected_tiles[i + 1];
        board_add_fill_vertices(board->board_outline_vertices, board->selected_tiles_vertices, (i + 2) * 24,
                                coords_to_point(x, y, board->board_dimension_x, board->board_dimension_y),
                                board->board_dimension_x, board->board_dimension_y);
    }
}
