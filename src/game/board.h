//
// Created by sterling on 6/26/24.
//

#ifndef BOARD_H
#define BOARD_H

#define BOARD_HEX_TILE_WIDTH 74
#define BOARD_HEX_TILE_HEIGHT 64

typedef struct Board
{
    int board_dimension_x;
    int board_dimension_y;

    int hovered_point;
    int selected_point;

    int mouse_tile_index_x;
    int mouse_tile_index_y;

    int selected_tile_index_x;
    int selected_tile_index_y;

    int *selected_tiles;
    int *tile_occupation_status;

    float *board_outline_vertices;
    unsigned int *board_outline_indices;
    float *board_tile_offsets;
    float board_border_positions[12];
    float board_border_uvs[12];
    float board_border_colors[24];
} Board;

Board *board_create(int dimension_x, int dimension_y);

void board_handle_tile_click(Board *board);

void board_update_hovered_tile(Board *board, float mouse_pos_x, float mouse_pos_y, float scale,
                               float adjusted_window_width, float adjusted_window_height);

#endif // BOARD_H
