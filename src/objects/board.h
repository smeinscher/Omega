//
// Created by sterling on 6/26/24.
//

#ifndef BOARD_H
#define BOARD_H

#include "../util/dynamic_array.h"
#include "planets.h"
#include "units.h"

#define BOARD_HEX_TILE_WIDTH 74
#define BOARD_HEX_TILE_HEIGHT 64

#define BOARD_UPDATE_BORDERS 0x1
#define BOARD_UPDATE_FILL 0x2

#define MAX_PLAYERS 4

typedef struct Board
{
    int board_dimension_x;
    int board_dimension_y;
    int player_count;

    int hovered_point;
    int selected_point;

    int mouse_tile_index_x;
    int mouse_tile_index_y;

    int selected_tile_index_x;
    int selected_tile_index_y;
    int last_selected_tile_index_x;
    int last_selected_tile_index_y;

    int board_borders_count;

    int board_update_flags;

    int board_current_turn;

    float *board_fill_positions;
    float *board_fill_colors;

    float *board_outline_vertices;
    unsigned int *board_outline_indices;
    float *board_border_positions;
    float *board_border_uvs;
    float *board_border_colors;

    DynamicIntArray *board_highlighted_path;
    DynamicIntArray *board_moveable_tiles;
    DynamicIntArray *board_attackable_tiles;

    Units *units;

    Planets *planets;
} Board;

Board *board_create(int dimension_x, int dimension_y, int player_count);

void board_reset(Board **board);

void board_reset_new_dimensions(Board **board, int new_board_dimension_x, int new_board_dimension_y);

void board_handle_tile_click(Board *board);

void board_update_hovered_tile(Board *board, float mouse_board_pos_x, float mouse_board_pos_y);

void board_update_fill_vertices(Board *board);

void board_update_border(Board *board);

void board_process_turn(Board *board);

void board_process_planet_orbit(Board *board);

BattleResult board_process_attack(Board *board, int defender_index, int attacker_index);

bool board_tile_is_occupied(Board *board, int x, int y);

void board_highlight_possible_unit_placement(Board *board, int station_index, int station_x, int station_y);

void board_clear(Board *board);

void board_destroy(Board *board);

#endif // BOARD_H
