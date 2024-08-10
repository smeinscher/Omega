//
// Created by sterling on 8/9/24.
//

#include "planets_shader.h"
#include "board.h"
#include <malloc.h>

PlanetsShader *planets_shader_create(int board_dimension_x, int board_dimension_y)
{
    PlanetsShader *planets_shader = malloc(sizeof(PlanetsShader));
    if (planets_shader == NULL)
    {
        printf("Error allocating planets_shader\n");
        return NULL;
    }

    planets_shader->planet_buffer_size = 1;
    planets_shader->planet_update_flags = 0;
    planets_shader->planet_tile_indices = NULL;
    planets_shader->planet_distance_from_sun = NULL;
    planets_shader->planet_type = NULL;
    planets_shader->planet_size = NULL;
    planets_shader->planet_positions = NULL;
    planets_shader->planet_colors = NULL;

    planets_shader->planet_tile_indices = malloc(sizeof(int) * 2);
    if (planets_shader->planet_tile_indices == NULL)
    {
        printf("Error allocating planet_tile_indices\n");
        planets_shader_destroy(planets_shader);
        return NULL;
    }
    planets_shader->planet_distance_from_sun = malloc(sizeof(int) * 2);
    if (planets_shader->planet_distance_from_sun == NULL)
    {
        printf("Error allocating planet_distance_from_sun\n");
        planets_shader_destroy(planets_shader);
        return NULL;
    }
    planets_shader->planet_type = malloc(sizeof(int));
    if (planets_shader->planet_type == NULL)
    {
        printf("Error allocating planet_type\n");
        planets_shader_destroy(planets_shader);
        return NULL;
    }
    planets_shader->planet_size = malloc(sizeof(float));
    if (planets_shader->planet_size == NULL)
    {
        printf("Error allocating planet_size\n");
        planets_shader_destroy(planets_shader);
        return NULL;
    }
    planets_shader->planet_positions = malloc(sizeof(float) * 12);
    if (planets_shader->planet_positions == NULL)
    {
        printf("Error allocating planet_positions\n");
        planets_shader_destroy(planets_shader);
        return NULL;
    }
    float center_x = ((float)board_dimension_x / 2.0f) * BOARD_HEX_TILE_WIDTH * 0.75f + BOARD_HEX_TILE_WIDTH / 8.0f;
    float center_y = ((float)board_dimension_y / 2.0f) * BOARD_HEX_TILE_HEIGHT;
    float size = 64.0f / 2.0f;
    planets_shader->planet_positions[0] = center_x - size;
    planets_shader->planet_positions[1] = center_y + size;
    planets_shader->planet_positions[2] = center_x + size;
    planets_shader->planet_positions[3] = center_y - size;
    planets_shader->planet_positions[4] = center_x - size;
    planets_shader->planet_positions[5] = center_y - size;
    planets_shader->planet_positions[6] = center_x - size;
    planets_shader->planet_positions[7] = center_y + size;
    planets_shader->planet_positions[8] = center_x + size;
    planets_shader->planet_positions[9] = center_y - size;
    planets_shader->planet_positions[10] = center_x + size;
    planets_shader->planet_positions[11] = center_y + size;

    planets_shader->planet_uvs = malloc(sizeof(float) * 12);
    if (planets_shader->planet_uvs == NULL)
    {
        printf("Error allocating planet_uvs\n");
        planets_shader_destroy(planets_shader);
        return NULL;
    }
    planets_shader->planet_uvs[0] = 0.0f;
    planets_shader->planet_uvs[1] = 1.0f;
    planets_shader->planet_uvs[2] = 1.0f;
    planets_shader->planet_uvs[3] = 0.0f;
    planets_shader->planet_uvs[4] = 0.0f;
    planets_shader->planet_uvs[5] = 0.0f;
    planets_shader->planet_uvs[6] = 0.0f;
    planets_shader->planet_uvs[7] = 1.0f;
    planets_shader->planet_uvs[8] = 1.0f;
    planets_shader->planet_uvs[9] = 0.0f;
    planets_shader->planet_uvs[10] = 1.0f;
    planets_shader->planet_uvs[11] = 1.0f;

    planets_shader->planet_colors = malloc(sizeof(float) * 24);
    if (planets_shader->planet_colors == NULL)
    {
        printf("Error allocating planet_colors\n");
        planets_shader_destroy(planets_shader);
        return NULL;
    }
    for (int i = 0; i < 6; i++)
    {
        planets_shader->planet_colors[i * 4] = 1.0f;
        planets_shader->planet_colors[i * 4 + 1] = 0.4f;
        planets_shader->planet_colors[i * 4 + 2] = 0.0f;
        planets_shader->planet_colors[i * 4 + 3] = 1.0f;
    }

    return planets_shader;
}

void planets_shader_clear(PlanetsShader *planets_shader)
{
    planets_shader->planet_buffer_size = 0;
    planets_shader->planet_update_flags = 0;
    free(planets_shader->planet_tile_indices);
    planets_shader->planet_tile_indices = NULL;
    free(planets_shader->planet_distance_from_sun);
    planets_shader->planet_distance_from_sun = NULL;
    free(planets_shader->planet_type);
    planets_shader->planet_type = NULL;
    free(planets_shader->planet_size);
    planets_shader->planet_size = NULL;
    free(planets_shader->planet_positions);
    planets_shader->planet_positions = NULL;
    free(planets_shader->planet_colors);
    planets_shader->planet_colors = NULL;
}

void planets_shader_destroy(PlanetsShader *planets_shader)
{
    planets_shader_clear(planets_shader);
    free(planets_shader);
}
