//
// Created by sterling on 6/26/24.
//

#include "board.h"

#include <stdlib.h>

float *board_vertices_create(unsigned int dimension_x, unsigned int dimension_y, int board_offset_x,
                             int board_offset_y, float tile_width, float tile_height)
{
    unsigned int num_vertices = dimension_y * 2 + dimension_y * (dimension_x * 2) + dimension_x * 2 + 1;
    float *board_vertices = malloc(sizeof(float) * num_vertices * 2);
    int index = 0;
    for (int i = 0; i < dimension_y; i++)
    {
        board_vertices[index++] = board_offset_x;
        board_vertices[index++] = board_offset_y + (float)i * tile_height + tile_height / 2.0f;
        for (int j = 0; j < dimension_x; j++)
        {
            const float y_offset = (float)(j % 2) * tile_height / 2.0f;
            board_vertices[index++] = board_offset_x + (float)j * 3.0f * tile_width / 4.0f + tile_width / 4.0f;
            board_vertices[index++] = board_offset_y + (float)i * tile_height + y_offset;
            board_vertices[index++] = board_offset_x + (float)j * 3.0f * tile_width / 4.0f + 3.0f * tile_width / 4.0f;
            board_vertices[index++] = board_offset_y + (float)i * tile_height + y_offset;
        }
        board_vertices[index++] = board_offset_x + (float)dimension_x * 3.0f * tile_width / 4.0f + tile_width / 4.0f;
        board_vertices[index++] = board_offset_y + (float)i * tile_height + tile_height / (
                                      1.0f + (float)(dimension_x % 2));
    }
    board_vertices[index++] = board_offset_x;
    board_vertices[index++] = board_offset_y + (float)dimension_y * tile_height + tile_height / 2.0f;
    for (int j = 0; j < dimension_x; j++)
    {
        const float y_offset = (float)(j % 2) * tile_height / 2.0f;
        board_vertices[index++] = board_offset_x + (float)(j) * 3.0f * tile_width / 4.0f + tile_width / 4.0f;
        board_vertices[index++] = board_offset_y + (float)dimension_y * tile_height + y_offset;
        board_vertices[index++] = board_offset_x + (float)j * 3.0f * tile_width / 4.0f + 3.0f * tile_width / 4.0f;
        board_vertices[index++] = board_offset_y + (float)dimension_y * tile_height + y_offset;
    }
    return board_vertices;
}

unsigned int *board_outline_indices_create(unsigned int dimension_x, unsigned int dimension_y)
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
