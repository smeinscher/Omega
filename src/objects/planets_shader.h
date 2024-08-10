//
// Created by sterling on 8/9/24.
//

#ifndef OMEGAAPP_PLANETS_SHADER_H
#define OMEGAAPP_PLANETS_SHADER_H

typedef struct PlanetsShader
{
    int planet_buffer_size;
    int planet_update_flags;

    int *planet_tile_indices;
    int *planet_distance_from_sun;
    int *planet_type;
    float *planet_size;
    float *planet_positions;
    float *planet_uvs;
    float *planet_colors;
} PlanetsShader;

PlanetsShader *planets_shader_create(int board_dimension_x, int board_dimension_y);

void planets_shader_clear(PlanetsShader *planets_shader);

void planets_shader_destroy(PlanetsShader *planets_shader);

#endif // OMEGAAPP_PLANETS_SHADER_H
