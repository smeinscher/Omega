//
// Created by sterling on 7/29/24.
//

#ifndef OMEGAAPP_UNITS_H
#define OMEGAAPP_UNITS_H

#include "../players/resources.h"
#include "../util/dynamic_array.h"

#define UNIT_UPDATE 0x1
#define UNIT_UPDATE_HEALTH 0x2

#define TOTAL_UNIT_TYPES 4

typedef enum UnitType
{
    WORKER,
    BATTLESHIP,
    DROID,
    STATION
} UnitType;

typedef enum BattleResult
{
    NO_UNITS_DESTROYED,
    DEFENDER_DESTROYED,
    ATTACKER_DESTROYED,
    BOTH_DESTROYED
} BattleResult;

enum MoveType
{
    REGULAR,
    ATTACK,
    RETREAT,
    SWAP
};

typedef struct Units
{
    int unit_buffer_size;
    int unit_update_flags;

    float unit_animation_progress;

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
    int *unit_tile_ownership_status;

    int *unit_type;

    int *unit_indices;

    DynamicIntArray unit_freed_indices;

    DynamicIntArray moves_unit_index;
    DynamicIntArray moves_list;
    DynamicIntArray move_type;
} Units;

Units *units_create(int board_dimension_x, int board_dimension_y);

void unit_realloc(Units *units);

void unit_clear_vertices(Units *units);

void unit_add(Units *units, int owner, int type, int x, int y, int board_dimension_x, int board_dimension_y);

void unit_remove(Units *units, int unit_index, int x, int y, int board_dimension_x);

BattleResult unit_attack(Units *units, int defender_index, int attacker_index);

void unit_replenish_health(Units *units, int unit_index);

void unit_replenish_movement(Units *units, int unit_index);

bool unit_purchase(int player_index, Resources *resources, Units *units, UnitType unit_type, int x, int y,
                   int board_dimension_x, int board_dimension_y);

bool unit_purchase_with_score(Units *units, int player_index, int *score, UnitType unit_type, int x, int y,
                              int board_dimension_x, int board_dimension_y);

bool unit_can_move(Units *units, int unit_index, int destination_x, int destination_y, int board_dimension_x);

void unit_move(Units *units, int unit_index, int current_x, int current_y, int destination_x, int destination_y,
               int board_dimension_x, int board_dimension_y);

void unit_stash_position(int x, int y);

void unit_swap(Units *units, int unit_index_a, int unit_index_b, int a_x, int a_y, int b_x, int b_y,
               int board_dimension_x, bool strategic_swap);

void unit_claim_territory(Units *units, int unit_index, int x, int y, int board_dimension_x, int board_dimension_y);

void unit_update_position(Units *units, int unit_index, int x, int y);

void unit_update_uv(Units *units, int unit_index);

void unit_update_color(Units *units, int unit_index);

void unit_update_health_position(Units *units, int unit_index);

void unit_update_health_uv(Units *units, int unit_index);

void unit_update_health_color(Units *units, int unit_index);

void unit_occupy_new_tile(Units *units, int unit_index, int previous_x, int previous_y, int new_x, int new_y,
                          int board_dimension_x);

void unit_add_movement_animation(Units *units, int unit_index, int start_x, int start_y, int end_x, int end_y,
                                 int move_type);

bool unit_animate_movement(Units *units);

void units_clear(Units *units);

void units_destroy(Units *units);

#endif // OMEGAAPP_UNITS_H
