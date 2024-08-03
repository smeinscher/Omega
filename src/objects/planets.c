//
// Created by sterling on 7/31/24.
//

#include "planets.h"
#include "../util/general_helpers.h"
#include "board.h"
#include <malloc.h>
#include <memory.h>

Planets *planets_create(int board_dimension_x, int board_dimension_y)
{
    Planets *planets = malloc(sizeof(Planets));
    if (planets == NULL)
    {
        printf("Error allocating planet\n");
        return NULL;
    }

    planets->planet_buffer_size = 0;
    planets->planet_update_flags = 0;
    planets->planet_tile_indices = NULL;
    planets->planet_distance_from_sun = NULL;
    planets->planet_type = NULL;
    planets->planet_size = NULL;
    planets->planet_positions = NULL;
    planets->planet_uvs = NULL;
    planets->planet_colors = NULL;

    planets->planet_tile_indices = malloc(sizeof(int) * 2);
    if (planets->planet_tile_indices == NULL)
    {
        printf("Error allocating planet_tile_indices\n");
        planets_destroy(planets);
        return NULL;
    }
    planets->planet_distance_from_sun = malloc(sizeof(int) * 2);
    if (planets->planet_distance_from_sun == NULL)
    {
        printf("Error allocating planet_distance_from_sun\n");
        planets_destroy(planets);
        return NULL;
    }
    planets->planet_type = malloc(sizeof(int));
    if (planets->planet_type == NULL)
    {
        printf("Error allocating planet_type\n");
        planets_destroy(planets);
        return NULL;
    }
    planets->planet_size = malloc(sizeof(float));
    if (planets->planet_size == NULL)
    {
        printf("Error allocating planet_size\n");
        planets_destroy(planets);
        return NULL;
    }
    planets->planet_positions = malloc(sizeof(float) * 12);
    if (planets->planet_positions == NULL)
    {
        printf("Error allocating planet_positions\n");
        planets_destroy(planets);
        return NULL;
    }
    planets->planet_uvs = malloc(sizeof(float) * 12);
    if (planets->planet_uvs == NULL)
    {
        printf("Error allocating planet_uvs\n");
        planets_destroy(planets);
        return NULL;
    }
    planets->planet_colors = malloc(sizeof(float) * 24);
    if (planets->planet_colors == NULL)
    {
        printf("Error allocating planet_colors\n");
        planets_destroy(planets);
        return NULL;
    }

    planet_add(planets, 0.6f, 3, 0, board_dimension_x / 2, board_dimension_y / 2);
    planet_add(planets, 0.15f, 0, 1, board_dimension_x / 2, board_dimension_y / 2 - 1);
    planet_add(planets, 0.15f, 2, 3, board_dimension_x / 2, board_dimension_y / 2 + 3);
    planet_add(planets, 0.15f, 4, 5, board_dimension_x / 2 + 5, board_dimension_y / 2);
    planet_add(planets, 0.15f, 1, 10, board_dimension_x / 2 - 10, board_dimension_y / 2);

    return planets;
}

void planet_orbit(Planets *planets)
{
    for (int i = 1; i < planets->planet_buffer_size; i++)
    {
        int x, y;
        x = planets->planet_tile_indices[i * 2] - planets->planet_tile_indices[0];
        y = planets->planet_tile_indices[i * 2 + 1] - planets->planet_tile_indices[1];
        int q, r;
        hex_grid_offset_to_axial(x, y, &q, &r);
        hex_grid_rotation_get_next(false, planets->planet_distance_from_sun[i], &q, &r);
        hex_grid_axial_to_offset(q, r, &x, &y);
        planets->planet_tile_indices[i * 2] = planets->planet_tile_indices[0] + x;
        planets->planet_tile_indices[i * 2 + 1] = planets->planet_tile_indices[1] + y;
        planet_update_position(planets, i);
    }
}

void planet_add(Planets *planets, float size, int type, int distance_from_sun, int x, int y)
{
    int new_planet_index = planets->planet_buffer_size++;
    if (planets->planet_buffer_size > 1)
    {
        realloc_int(&planets->planet_tile_indices, planets->planet_buffer_size * 2);
        realloc_int(&planets->planet_distance_from_sun, planets->planet_buffer_size);
        realloc_int(&planets->planet_type, planets->planet_buffer_size);
        realloc_float(&planets->planet_size, planets->planet_buffer_size);
        realloc_float(&planets->planet_positions, planets->planet_buffer_size * 12);
        realloc_float(&planets->planet_uvs, planets->planet_buffer_size * 12);
        realloc_float(&planets->planet_colors, planets->planet_buffer_size * 24);
    }
    planets->planet_tile_indices[new_planet_index * 2] = x;
    planets->planet_tile_indices[new_planet_index * 2 + 1] = y;
    planets->planet_distance_from_sun[new_planet_index] = distance_from_sun;
    planets->planet_type[new_planet_index] = type;
    planets->planet_size[new_planet_index] = size;

    planet_update_position(planets, new_planet_index);
    planet_update_uv(planets, new_planet_index);
    planet_update_color(planets, new_planet_index);
}

void planet_remove(Planets *planets, int planet_index)
{
    planets->planet_buffer_size--;
    planets->planet_tile_indices[planet_index * 2] = -1;
    planets->planet_tile_indices[planet_index * 2 + 1] = -1;
    planets->planet_size[planet_index] = 0;
    memset(planets->planet_positions + planet_index * 12, 0, sizeof(float) * 12);
    memset(planets->planet_uvs + planet_index * 12, 0, sizeof(float) * 12);
    memset(planets->planet_colors + planet_index * 24, 0, sizeof(float) * 24);

    planets->planet_update_flags |= PLANET_UPDATE;
}

void planet_update_position(Planets *planets, int planet_index)
{
    int x = planets->planet_tile_indices[planet_index * 2];
    int y = planets->planet_tile_indices[planet_index * 2 + 1];
    float size_x = BOARD_HEX_TILE_HEIGHT * planets->planet_size[planet_index];
    float size_y = BOARD_HEX_TILE_HEIGHT * planets->planet_size[planet_index];
    float offset_x = (BOARD_HEX_TILE_WIDTH - size_x) / 2.0f;
    float offset_y = ((1.0f - planets->planet_size[planet_index]) * BOARD_HEX_TILE_HEIGHT) / 2.0f;
    planets->planet_positions[planet_index * 12] = (float)x * BOARD_HEX_TILE_WIDTH * 0.75f + offset_x;
    planets->planet_positions[planet_index * 12 + 1] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f + offset_y + size_y;
    planets->planet_positions[planet_index * 12 + 2] = (float)x * BOARD_HEX_TILE_WIDTH * 0.75f + offset_x + size_x;
    planets->planet_positions[planet_index * 12 + 3] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f + offset_y;
    planets->planet_positions[planet_index * 12 + 4] = (float)x * BOARD_HEX_TILE_WIDTH * 0.75f + offset_x;
    planets->planet_positions[planet_index * 12 + 5] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f + offset_y;
    planets->planet_positions[planet_index * 12 + 6] = (float)x * BOARD_HEX_TILE_WIDTH * 0.75f + offset_x;
    planets->planet_positions[planet_index * 12 + 7] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f + offset_y + size_y;
    planets->planet_positions[planet_index * 12 + 8] = (float)x * BOARD_HEX_TILE_WIDTH * 0.75f + offset_x + size_x;
    planets->planet_positions[planet_index * 12 + 9] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f + offset_y;
    planets->planet_positions[planet_index * 12 + 10] = (float)x * BOARD_HEX_TILE_WIDTH * 0.75f + offset_x + size_x;
    planets->planet_positions[planet_index * 12 + 11] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f + offset_y + size_y;

    planets->planet_update_flags |= PLANET_UPDATE;
}

void planet_update_uv(Planets *planets, int planet_index)
{
    // TODO: replace 5.0f and 6.0f with variables
    planets->planet_uvs[planet_index * 12] = 0.0f;
    planets->planet_uvs[planet_index * 12 + 1] = ((float)planets->planet_type[planet_index] + 1.0f) / 5.0f;
    planets->planet_uvs[planet_index * 12 + 2] = 1.0f / 6.0f;
    planets->planet_uvs[planet_index * 12 + 3] = (float)planets->planet_type[planet_index] / 5.0f;
    planets->planet_uvs[planet_index * 12 + 4] = 0.0f;
    planets->planet_uvs[planet_index * 12 + 5] = (float)planets->planet_type[planet_index] / 5.0f;
    planets->planet_uvs[planet_index * 12 + 6] = 0.0f;
    planets->planet_uvs[planet_index * 12 + 7] = ((float)planets->planet_type[planet_index] + 1.0f) / 5.0f;
    planets->planet_uvs[planet_index * 12 + 8] = 1.0f / 6.0f;
    planets->planet_uvs[planet_index * 12 + 9] = (float)planets->planet_type[planet_index] / 5.0f;
    planets->planet_uvs[planet_index * 12 + 10] = 1.0f / 6.0f;
    planets->planet_uvs[planet_index * 12 + 11] = ((float)planets->planet_type[planet_index] + 1.0f) / 5.0f;
    planets->planet_update_flags |= PLANET_UPDATE;
}

void planet_update_color(Planets *planets, int planet_index)
{
    for (int i = 0; i < 6; i++)
    {
        planets->planet_colors[planet_index * 24 + i * 4] = 1.0f;
        planets->planet_colors[planet_index * 24 + i * 4 + 1] = 1.0f;
        planets->planet_colors[planet_index * 24 + i * 4 + 2] = 1.0f;
        planets->planet_colors[planet_index * 24 + i * 4 + 3] = 1.0f;
    }
    planets->planet_update_flags |= PLANET_UPDATE;
}

void planets_clear(Planets *planets)
{
    planets->planet_buffer_size = 0;
    planets->planet_update_flags = 0;
    free(planets->planet_tile_indices);
    planets->planet_tile_indices = NULL;
    free(planets->planet_distance_from_sun);
    planets->planet_distance_from_sun = NULL;
    free(planets->planet_type);
    planets->planet_type = NULL;
    free(planets->planet_size);
    planets->planet_size = NULL;
    free(planets->planet_positions);
    planets->planet_positions = NULL;
    free(planets->planet_uvs);
    planets->planet_uvs = NULL;
    free(planets->planet_colors);
    planets->planet_colors = NULL;
}

void planets_destroy(Planets *planets)
{
    planets_clear(planets);
    free(planets);
}
