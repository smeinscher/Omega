//
// Created by sterling on 6/26/24.
//

#ifndef BOARD_H
#define BOARD_H

float *board_vertices_create(unsigned int dimension_x, unsigned int dimension_y, int board_offset_x,
                             int board_offset_y, float tile_width, float tile_height);

unsigned int *board_outline_indices_create(unsigned int dimension_x, unsigned int dimension_y);

unsigned int *board_fill_indices_create();

#endif //BOARD_H
