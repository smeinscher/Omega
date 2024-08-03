//
// Created by sterling on 7/29/24.
//

#ifndef OMEGAAPP_GENERAL_HELPERS_H
#define OMEGAAPP_GENERAL_HELPERS_H

#include "../objects/board.h"
#include "dynamic_array.h"
#include <stdbool.h>
#include <stddef.h>

void realloc_int(int **int_ptr, size_t size);

void realloc_float(float **float_ptr, size_t size);

int max_int(int a, int b);

void hex_grid_axial_to_offset(int q, int r, int *x, int *y);

void hex_grid_offset_to_axial(int x, int y, int *q, int *r);

int hex_grid_get_axial_distance(int start_q, int start_r, int target_q, int target_r);

void hex_grid_rotation_get_next(bool clockwise, int distance_from_center, int *q, int *r);

DynamicIntArray *hex_grid_find_path(Board *board, int start_x, int start_y, int target_x, int target_y,
                                    int board_dimension_x, int board_dimension_y);

DynamicIntArray *hex_grid_possible_moves(Board *board, int unit_index, int unit_x, int unit_y);

DynamicIntArray *hex_grid_possible_attacks(Board *board, int unit_index, int unit_x, int unit_y);

#endif // OMEGAAPP_GENERAL_HELPERS_H
