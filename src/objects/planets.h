//
// Created by sterling on 7/31/24.
//

#ifndef OMEGAAPP_PLANETS_H
#define OMEGAAPP_PLANETS_H

#include <stdbool.h>
#define PLANET_UPDATE 0x1

#define PLANET_TOTAL_TYPES 16

enum PlanetType
{
    SUN,
    MERCURY,
    VENUS,
    EARTH,
    MARS,
    ICY,
};

typedef struct Planets
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
} Planets;

Planets *planets_create(int board_dimension_x, int board_dimension_y, int planet_count);

void planet_orbit(Planets *planets);

void planet_add(Planets *planets, float size, int type, int distance_from_sun, int x, int y);

void planet_remove(Planets *planets, int planet_index);

void planet_update_position(Planets *planets, int planet_index);

void planet_update_uv(Planets *planets, int planet_index);

void planet_update_color(Planets *planets, int planet_index);

int planet_find_closest(Planets *planets, int x, int y);

bool planet_on_tile(Planets *planets, int x, int y);

void planets_clear(Planets *planets);

void planets_destroy(Planets *planets);

#endif // OMEGAAPP_PLANETS_H
