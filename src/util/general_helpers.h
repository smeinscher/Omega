//
// Created by sterling on 7/29/24.
//

#ifndef OMEGAAPP_GENERAL_HELPERS_H
#define OMEGAAPP_GENERAL_HELPERS_H

#include <stdbool.h>
#include <stddef.h>

void realloc_int(int **int_ptr, size_t size);

void realloc_float(float **float_ptr, size_t size);

void hex_grid_axial_to_offset(int q, int r, int *x, int *y);

void hex_grid_offset_to_axial(int x, int y, int *q, int *r);

void hex_grid_get_next(bool clockwise, int distance_from_center, int *q, int *r);

#endif // OMEGAAPP_GENERAL_HELPERS_H
