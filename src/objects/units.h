//
// Created by sterling on 7/29/24.
//

#ifndef OMEGAAPP_UNITS_H
#define OMEGAAPP_UNITS_H

#include "../util/dynamic_array.h"

#define UNIT_UPDATE 0x1
#define UNIT_UPDATE_HEALTH 0x2

#define TOTAL_UNIT_TYPES 4

enum UnitType
{
    DROID,
    BATTLESHIP,
    SCOUT,
    STATION
};

enum BattleResult
{
    NO_UNITS_DESTROYED,
    DEFENDER_DESTROYED,
    ATTACKER_DESTROYED,
    BOTH_DESTROYED
};

typedef struct Units
{
    int unit_buffer_size;
    int unit_update_flags;

    int *unit_owner;
    float *unit_positions;
    float *unit_uvs;
    float *unit_colors;

    float *unit_health;
    float *unit_health_positions;
    float *unit_health_uvs;
    float *unit_health_colors;

    float *unit_movement_points;

    int *unit_tile_occupation_status;

    int *unit_type;

    DynamicIntArray unit_freed_indices;

} Units;

Units *units_create(int board_dimension_x, int board_dimension_y);

void unit_realloc(Units *units);

void unit_clear_vertices(Units *units);

void unit_add(Units *units, int owner, int type, int x, int y, int board_dimension_x);

void unit_remove(Units *units, int unit_index, int x, int y, int board_dimension_x);

enum BattleResult unit_attack(Units *units, int defender_index, int attacker_index);

void unit_update_position(Units *units, int unit_index, int x, int y);

void unit_update_uv(Units *units, int unit_index);

void unit_update_color(Units *units, int unit_index);

void unit_update_health_position(Units *units, int unit_index);

void unit_update_health_uv(Units *units, int unit_index);

void unit_update_health_color(Units *units, int unit_index);

void units_clear(Units *units);

void units_destroy(Units *units);

#endif // OMEGAAPP_UNITS_H
