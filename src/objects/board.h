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

    int board_borders_count;

    int unit_count;

    int *selected_tiles;
    float *board_fill_positions;
    float *board_fill_colors;
    int *tile_occupation_status;
    int *tile_ownership_status;

    int *board_unit_owner;
    float *board_unit_positions;
    float *board_unit_uvs;
    float *board_unit_colors;

    float *board_outline_vertices;
    unsigned int *board_outline_indices;
    float *board_border_positions;
    float *board_border_uvs;
    float *board_border_colors;
} Board;

Board *board_create(int dimension_x, int dimension_y);

void board_handle_tile_click(Board *board);

void board_update_hovered_tile(Board *board, float mouse_board_pos_x, float mouse_board_pos_y);

void board_update_fill_vertices(Board *board);

void board_update_border(Board *board);

void board_update_unit_position(Board *board, int unit_index, int x, int y);

void board_clear(Board *board);

void board_destroy(Board *board);

#endif // BOARD_H
