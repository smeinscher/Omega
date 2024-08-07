//
// Created by sterling on 7/29/24.
//

#include "units.h"
#include "../util/general_helpers.h"
#include "board.h"
#include <cglm/util.h>
#include <memory.h>
#include <stdlib.h>

static float g_health_bar_size_x = BOARD_HEX_TILE_WIDTH / 2.0f;
static float g_health_bar_size_y = BOARD_HEX_TILE_HEIGHT / 12.0f;
static float g_health_bar_offset_x = BOARD_HEX_TILE_WIDTH / 4.0f;
static float g_health_bar_offset_y = BOARD_HEX_TILE_HEIGHT / 2.0f - (BOARD_HEX_TILE_HEIGHT / 3.0f);

// TODO: set this up in a file
static float unit_base_damage[] = {0.0f, 400.0f, 160.0f, 0.0f};
static float unit_base_health[] = {50.0f, 400.0f, 100.0f, 500.0f};
static float unit_base_movement[] = {4.0f, 1.0f, 2.0f, 0.0f};
static int unit_base_resource_cost[] = {0, 50, 500, 40, 150, 3000, 10, 50, 1250, 0, 0, 0};

static int g_stash_x = -1;
static int g_stash_y = -1;

Units *units_create(int board_dimension_x, int board_dimension_y)
{
    Units *units = malloc(sizeof(Units));
    if (units == NULL)
    {
        printf("Error allocating units\n");
        return NULL;
    }

    units->unit_buffer_size = 0;
    units->unit_update_flags = 0;
    units->unit_animation_progress = 0.0f;
    units->unit_owner = NULL;
    units->unit_health = NULL;
    units->unit_positions = NULL;
    units->unit_uvs = NULL;
    units->unit_colors = NULL;
    units->unit_health_positions = NULL;
    units->unit_health_uvs = NULL;
    units->unit_health_colors = NULL;
    units->unit_tile_occupation_status = NULL;
    units->unit_tile_ownership_status = NULL;
    units->unit_movement_points = NULL;
    units->unit_type = NULL;
    units->unit_indices = NULL;

    da_int_init(&units->unit_freed_indices, 4);

    da_int_init(&units->moves_unit_index, 1);
    da_int_init(&units->moves_list, 4);
    da_int_init(&units->move_type, 1);

    units->unit_owner = malloc(sizeof(int));
    if (units->unit_owner == NULL)
    {
        printf("Error allocating unit_owner\n");
        units_destroy(units);
        return NULL;
    }
    units->unit_health = malloc(sizeof(float));
    if (units->unit_health == NULL)
    {
        printf("Error allocating board_unit_health\n");
        units_destroy(units);
        return NULL;
    }
    units->unit_positions = malloc(sizeof(float) * 12);
    if (units->unit_positions == NULL)
    {
        printf("Error allocating board_unit_positions\n");
        units_destroy(units);
        return NULL;
    }

    units->unit_uvs = malloc(sizeof(float) * 12);
    if (units->unit_uvs == NULL)
    {
        printf("Error allocating board_unit_uvs\n");
        units_destroy(units);
        return NULL;
    }

    units->unit_colors = malloc(sizeof(float) * 24);
    if (units->unit_colors == NULL)
    {
        printf("Error allocating board_unit_colors\n");
        units_destroy(units);
        return NULL;
    }

    units->unit_health_positions = malloc(sizeof(float) * 12);
    if (units->unit_health_positions == NULL)
    {
        printf("Error allocating board_unit_health_positions\n");
        units_destroy(units);
        return NULL;
    }

    units->unit_health_uvs = malloc(sizeof(float) * 12);
    if (units->unit_health_uvs == NULL)
    {
        printf("Error allocating board_unit_health_uvs\n");
        units_destroy(units);
        return NULL;
    }

    units->unit_health_colors = malloc(sizeof(float) * 24);
    if (units->unit_health_colors == NULL)
    {
        printf("Error allocating board_unit_health_colors\n");
        units_destroy(units);
        return NULL;
    }

    units->unit_tile_occupation_status = malloc(sizeof(int) * board_dimension_x * board_dimension_y);
    if (units->unit_tile_occupation_status == NULL)
    {
        printf("Error allocating tile_occupation_status\n");
        units_destroy(units);
        return NULL;
    }
    for (int i = 0; i < board_dimension_x * board_dimension_y; i++)
    {
        units->unit_tile_occupation_status[i] = -1;
    }

    units->unit_tile_ownership_status = malloc(sizeof(int) * board_dimension_x * board_dimension_y);
    if (units->unit_tile_ownership_status == NULL)
    {
        printf("Error allocating unit_tile_ownership_status\n");
        units_destroy(units);
        return NULL;
    }
    memset(units->unit_tile_ownership_status, 0, sizeof(int) * board_dimension_x * board_dimension_y);

    units->unit_movement_points = malloc(sizeof(float));
    if (units->unit_movement_points == NULL)
    {
        printf("Error allocating unit_movement_points\n");
        units_destroy(units);
        return NULL;
    }

    units->unit_type = malloc(sizeof(int));
    if (units->unit_type == NULL)
    {
        printf("Error allocating unit_type\n");
        units_destroy(units);
        return NULL;
    }

    units->unit_indices = malloc(sizeof(int) * 2);
    if (units->unit_indices == NULL)
    {
        printf("Error allocating unit_indices\n");
        units_destroy(units);
        return NULL;
    }

    unit_add(units, 1, DROID, 1, 0, board_dimension_x, board_dimension_y);
    unit_add(units, 1, STATION, 1, 1, board_dimension_x, board_dimension_y);
    unit_add(units, 2, DROID, 1, board_dimension_y - 1, board_dimension_x, board_dimension_y);
    unit_add(units, 2, STATION, 1, board_dimension_y - 2, board_dimension_x, board_dimension_y);
    unit_add(units, 3, DROID, board_dimension_x - 2, 0, board_dimension_x, board_dimension_y);
    unit_add(units, 3, STATION, board_dimension_x - 2, 1, board_dimension_x, board_dimension_y);
    unit_add(units, 4, DROID, board_dimension_x - 2, board_dimension_y - 1, board_dimension_x, board_dimension_y);
    unit_add(units, 4, STATION, board_dimension_x - 2, board_dimension_y - 2, board_dimension_x, board_dimension_y);

    units->unit_tile_ownership_status[1] = 1;
    units->unit_tile_ownership_status[board_dimension_x] = 1;
    units->unit_tile_ownership_status[board_dimension_x * 2] = 1;
    units->unit_tile_ownership_status[board_dimension_x * 2 + 1] = 1;
    units->unit_tile_ownership_status[board_dimension_x * 2 + 2] = 1;
    units->unit_tile_ownership_status[board_dimension_x * 1 + 2] = 1;
    units->unit_tile_ownership_status[board_dimension_x * 1 + 1] = 1;

    units->unit_tile_ownership_status[(board_dimension_y - 3) * board_dimension_x + 1] = 2;
    units->unit_tile_ownership_status[(board_dimension_y - 2) * board_dimension_x] = 2;
    units->unit_tile_ownership_status[(board_dimension_y - 1) * board_dimension_x] = 2;
    units->unit_tile_ownership_status[(board_dimension_y - 1) * board_dimension_x + 1] = 2;
    units->unit_tile_ownership_status[(board_dimension_y - 1) * board_dimension_x + 2] = 2;
    units->unit_tile_ownership_status[(board_dimension_y - 2) * board_dimension_x + 2] = 2;
    units->unit_tile_ownership_status[(board_dimension_y - 2) * board_dimension_x + 1] = 2;

    units->unit_tile_ownership_status[board_dimension_x - 2] = 3;
    units->unit_tile_ownership_status[2 * board_dimension_x - 3] = 3;
    units->unit_tile_ownership_status[3 * board_dimension_x - 3] = 3;
    units->unit_tile_ownership_status[3 * board_dimension_x - 2] = 3;
    units->unit_tile_ownership_status[3 * board_dimension_x - 1] = 3;
    units->unit_tile_ownership_status[2 * board_dimension_x - 1] = 3;
    units->unit_tile_ownership_status[2 * board_dimension_x - 2] = 3;

    units->unit_tile_ownership_status[(board_dimension_y - 2) * board_dimension_x - 2] = 4;
    units->unit_tile_ownership_status[(board_dimension_y - 1) * board_dimension_x - 3] = 4;
    units->unit_tile_ownership_status[board_dimension_y * board_dimension_x - 3] = 4;
    units->unit_tile_ownership_status[board_dimension_y * board_dimension_x - 2] = 4;
    units->unit_tile_ownership_status[board_dimension_y * board_dimension_x - 1] = 4;
    units->unit_tile_ownership_status[(board_dimension_y - 1) * board_dimension_x - 1] = 4;
    units->unit_tile_ownership_status[(board_dimension_y - 1) * board_dimension_x - 2] = 4;

    return units;
}

void unit_realloc(Units *units)
{
    realloc_int(&units->unit_owner, units->unit_buffer_size);
    realloc_float(&units->unit_health, units->unit_buffer_size);
    realloc_float(&units->unit_positions, units->unit_buffer_size * 12);
    realloc_float(&units->unit_uvs, units->unit_buffer_size * 12);
    realloc_float(&units->unit_colors, units->unit_buffer_size * 24);
    realloc_float(&units->unit_health_positions, units->unit_buffer_size * 12);
    realloc_float(&units->unit_health_uvs, units->unit_buffer_size * 12);
    realloc_float(&units->unit_health_colors, units->unit_buffer_size * 24);
    realloc_float(&units->unit_movement_points, units->unit_buffer_size);
    realloc_int(&units->unit_type, units->unit_buffer_size);
    realloc_int(&units->unit_indices, units->unit_buffer_size * 2);
}

void unit_clear_vertices(Units *units)
{
    memset(units->unit_positions, 0, sizeof(float) * units->unit_buffer_size * 12);
    memset(units->unit_uvs, 0, sizeof(float) * units->unit_buffer_size * 12);
    memset(units->unit_colors, 0, sizeof(float) * units->unit_buffer_size * 24);
    memset(units->unit_health_positions, 0, sizeof(float) * units->unit_buffer_size * 12);
    memset(units->unit_health_uvs, 0, sizeof(float) * units->unit_buffer_size * 12);
    memset(units->unit_health_colors, 0, sizeof(float) * units->unit_buffer_size * 24);
}

void unit_add(Units *units, int owner, int type, int x, int y, int board_dimension_x, int board_dimension_y)
{
    int new_unit_index;
    if (units->unit_freed_indices.used == 0)
    {
        new_unit_index = units->unit_buffer_size++;
        if (units->unit_buffer_size > 1)
        {
            unit_realloc(units);
        }
    }
    else
    {
        new_unit_index = da_int_pop_front(&units->unit_freed_indices);
    }

    units->unit_tile_occupation_status[y * board_dimension_x + x] = new_unit_index;
    units->unit_owner[new_unit_index] = owner;
    units->unit_health[new_unit_index] = unit_base_health[type];
    units->unit_movement_points[new_unit_index] = unit_base_movement[type];
    units->unit_type[new_unit_index] = type;
    units->unit_indices[new_unit_index * 2] = x;
    units->unit_indices[new_unit_index * 2 + 1] = y;

    unit_update_position(units, new_unit_index, x, y);

    unit_update_health_position(units, new_unit_index);

    unit_update_uv(units, new_unit_index);

    unit_update_health_uv(units, new_unit_index);

    unit_update_color(units, new_unit_index);

    unit_update_health_color(units, new_unit_index);

    //    unit_claim_territory(units, new_unit_index, x, y, board_dimension_x, board_dimension_y);
}

void unit_remove(Units *units, int unit_index, int x, int y, int board_dimension_x)
{
    da_int_push_back(&units->unit_freed_indices, unit_index);
    units->unit_tile_occupation_status[y * board_dimension_x + x] = -1;
    units->unit_owner[unit_index] = -1;
    units->unit_health[unit_index] = 0.0f;
    memset(units->unit_positions + unit_index * 12, 0, 12 * sizeof(float));
    memset(units->unit_uvs + unit_index * 12, 0, 12 * sizeof(float));
    memset(units->unit_colors + unit_index * 24, 0, 24 * sizeof(float));
    memset(units->unit_health_positions + unit_index * 12, 0, 12 * sizeof(float));
    memset(units->unit_health_uvs + unit_index * 12, 0, 12 * sizeof(float));
    memset(units->unit_health_colors + unit_index * 24, 0, 24 * sizeof(float));

    units->unit_indices[unit_index * 2] = -1;
    units->unit_indices[unit_index * 2 + 1] = -1;

    units->unit_update_flags |= UNIT_UPDATE | UNIT_UPDATE_HEALTH;
}

BattleResult unit_attack(Units *units, int defender_index, int attacker_index)
{
    units->unit_movement_points[attacker_index] = 0.0f;
    // TODO: damage based on exp/level, and current health
    // TODO: also, find better random
    float attacker_base_damage = unit_base_damage[units->unit_type[attacker_index]];
    units->unit_health[defender_index] -= ((float)(rand() % ((int)attacker_base_damage * 100)) + attacker_base_damage) /
        unit_base_damage[units->unit_type[attacker_index]];
    if (units->unit_type[defender_index] != WORKER && units->unit_type[defender_index] != STATION)
    {
        float defender_base_damage = unit_base_damage[units->unit_type[defender_index]];
        units->unit_health[attacker_index] -=
            ((float)(rand() % ((int)defender_base_damage * 100)) + defender_base_damage) /
            unit_base_damage[units->unit_type[defender_index]];
    }
    if (units->unit_health[defender_index] > 0.0f && units->unit_health[attacker_index] > 0.0f)
    {
        unit_update_health_position(units, defender_index);
        unit_update_health_position(units, attacker_index);
        return NO_UNITS_DESTROYED;
    }
    if (units->unit_health[defender_index] <= 0.0f && units->unit_health[attacker_index] > 0.0f)
    {
        unit_update_health_position(units, attacker_index);
        return DEFENDER_DESTROYED;
    }
    if (units->unit_health[defender_index] >= 0.0f && units->unit_health[attacker_index] <= 0.0f)
    {
        unit_update_health_position(units, defender_index);
        return ATTACKER_DESTROYED;
    }

    return BOTH_DESTROYED;
}

void unit_replenish_health(Units *units, int unit_index)
{
    units->unit_health[unit_index] += 25.0f;
    if (units->unit_health[unit_index] >= unit_base_health[units->unit_type[unit_index]])
    {
        units->unit_health[unit_index] = unit_base_health[units->unit_type[unit_index]];
    }
    unit_update_health_position(units, unit_index);
}

void unit_replenish_movement(Units *units, int unit_index)
{
    units->unit_movement_points[unit_index] = unit_base_movement[units->unit_type[unit_index]];
}

bool unit_purchase(int player_index, Resources *resources, Units *units, UnitType unit_type, int x, int y,
                   int board_dimension_x, int board_dimension_y)
{
    if (resources->radioactive_material_count[player_index] >= unit_base_resource_cost[unit_type * 3] &&
        resources->rare_metals_count[player_index] >= unit_base_resource_cost[unit_type * 3 + 1] &&
        resources->common_metals_count[player_index] >= unit_base_resource_cost[unit_type * 3 + 2])
    {
        resources->radioactive_material_count[player_index] -= unit_base_resource_cost[unit_type * 3];
        resources->rare_metals_count[player_index] -= unit_base_resource_cost[unit_type * 3 + 1];
        resources->common_metals_count[player_index] -= unit_base_resource_cost[unit_type * 3 + 2];
        unit_add(units, player_index + 1, unit_type, x, y, board_dimension_x, board_dimension_y);
        return true;
    }
    return false;
}

bool unit_purchase_with_score(Units *units, int player_index, int *score, UnitType unit_type, int x, int y,
                              int board_dimension_x, int board_dimension_y)
{
    if (unit_type == DROID && *score >= 150)
    {
        unit_add(units, player_index + 1, unit_type, x, y, board_dimension_x, board_dimension_y);
        *score -= 150;
        return true;
    }
    if (unit_type == BATTLESHIP && *score >= 410)
    {
        unit_add(units, player_index + 1, unit_type, x, y, board_dimension_x, board_dimension_y);
        *score -= 410;
        return true;
    }
    return false;
}

bool unit_can_move(Units *units, int unit_index, int destination_x, int destination_y, int board_dimension_x)
{
    return units->unit_tile_occupation_status[destination_y * board_dimension_x + destination_x] == -1 ||
           units->unit_tile_occupation_status[destination_y * board_dimension_x + destination_x] == unit_index;
}

void unit_move(Units *units, int unit_index, DynamicIntArray *move_path,
               int end_x, int end_y, int board_dimension_x, int board_dimension_y)
{
    //    units->unit_tile_occupation_status[current_y * board_dimension_x + current_x] = -1;
    //    units->unit_tile_occupation_status[destination_y * board_dimension_x + destination_x] = unit_index;
    //    units->unit_indices[unit_index * 2] = destination_x;
    //    units->unit_indices[unit_index * 2 + 1] = destination_y;
    for (int i = 0; i < move_path->used; i += 2)
    {
        int x = move_path->array[i];
        int y = move_path->array[i + 1];
        unit_claim_territory(units, unit_index, x, y, board_dimension_x,
                             board_dimension_y);
    }
    unit_claim_territory(units, unit_index, end_x, end_y, board_dimension_x,
                         board_dimension_y);
    //    printf("Unit %d of Player %d moved from %d, %d to %d, %d\n", unit_index, units->unit_owner[unit_index],
    //    current_x,
    //           current_y, destination_x, destination_y);
}

void unit_stash_position(int x, int y)
{
    g_stash_x = x;
    g_stash_y = y;
}

void unit_swap(Units *units, int unit_index_a, int unit_index_b, int a_x, int a_y, int b_x, int b_y,
               int board_dimension_x, bool strategic_swap)
{
    units->unit_tile_occupation_status[a_y * board_dimension_x + a_x] = unit_index_b;
    units->unit_tile_occupation_status[b_y * board_dimension_x + b_x] = unit_index_a;
    if (unit_index_a != -1)
    {
        unit_add_movement_animation(units, unit_index_a, a_x, a_y, b_x, b_y, REGULAR);
        unit_occupy_new_tile(units, unit_index_a, a_x, a_y, b_x, b_y, board_dimension_x);
    }
    if (unit_index_b != -1)
    {
        unit_add_movement_animation(units, unit_index_b, b_x, b_y, a_x, a_y, REGULAR);
        unit_occupy_new_tile(units, unit_index_b, b_x, b_y, a_x, a_y, board_dimension_x);
    }
    if (strategic_swap)
    {
        int start_q, start_r, target_q, target_r;
        hex_grid_offset_to_axial(a_x, a_y, &start_q, &start_r);
        hex_grid_offset_to_axial(b_x, b_y, &target_q, &target_r);
        units->unit_movement_points[unit_index_a] -=
            (float)hex_grid_get_axial_distance(start_q, start_r, target_q, target_r);
        units->unit_movement_points[unit_index_b] -=
            (float)hex_grid_get_axial_distance(target_q, target_r, start_q, start_r);
    }
    if (unit_index_a != -1)
    {
        units->unit_indices[unit_index_a * 2] = b_x;
        units->unit_indices[unit_index_a * 2 + 1] = b_y;
    }
    if (unit_index_b != -1)
    {
        units->unit_indices[unit_index_b * 2] = a_x;
        units->unit_indices[unit_index_b * 2 + 1] = a_y;
    }
}

void unit_claim_territory(Units *units, int unit_index, int x, int y, int board_dimension_x, int board_dimension_y)
{
    units->unit_tile_ownership_status[y * board_dimension_x + x] = units->unit_owner[unit_index];
    // Tiles around unit will be taken over, unless occupied
    if (y > 0 && units->unit_tile_occupation_status[(y - 1) * board_dimension_x + x] == -1)
    {
        units->unit_tile_ownership_status[(y - 1) * board_dimension_x + x] = units->unit_owner[unit_index];
    }
    if (y < board_dimension_y - 1 && units->unit_tile_occupation_status[(y + 1) * board_dimension_x + x] == -1)
    {
        units->unit_tile_ownership_status[(y + 1) * board_dimension_x + x] = units->unit_owner[unit_index];
    }
    if (x > 0 && units->unit_tile_occupation_status[y * board_dimension_x + x - 1] == -1)
    {
        units->unit_tile_ownership_status[y * board_dimension_x + x - 1] = units->unit_owner[unit_index];
    }
    if (x < board_dimension_x - 1 && units->unit_tile_occupation_status[y * board_dimension_x + x + 1] == -1)
    {
        units->unit_tile_ownership_status[y * board_dimension_x + x + 1] = units->unit_owner[unit_index];
    }
    if (x % 2 == 0)
    {
        if (x > 0 && y > 0 && units->unit_tile_occupation_status[(y - 1) * board_dimension_x + x - 1] == -1)
        {
            units->unit_tile_ownership_status[(y - 1) * board_dimension_x + x - 1] = units->unit_owner[unit_index];
        }
        if (x < board_dimension_x - 1 && y > 0 &&
            units->unit_tile_occupation_status[(y - 1) * board_dimension_x + x + 1] == -1)
        {
            units->unit_tile_ownership_status[(y - 1) * board_dimension_x + x + 1] = units->unit_owner[unit_index];
        }
    }
    else
    {
        if (x > 0 && y < board_dimension_y - 1 &&
            units->unit_tile_occupation_status[(y + 1) * board_dimension_x + x - 1] == -1)
        {
            units->unit_tile_ownership_status[(y + 1) * board_dimension_x + x - 1] = units->unit_owner[unit_index];
        }
        if (x < board_dimension_x - 1 && y < board_dimension_y - 1 &&
            units->unit_tile_occupation_status[(y + 1) * board_dimension_x + x + 1] == -1)
        {
            units->unit_tile_ownership_status[(y + 1) * board_dimension_x + x + 1] = units->unit_owner[unit_index];
        }
    }
}

void unit_update_position(Units *units, int unit_index, int x, int y)
{
    units->unit_positions[unit_index * 12] = (float)x * BOARD_HEX_TILE_WIDTH * 0.75f;
    units->unit_positions[unit_index * 12 + 1] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f + BOARD_HEX_TILE_HEIGHT;
    units->unit_positions[unit_index * 12 + 2] = (float)x * BOARD_HEX_TILE_WIDTH * 0.75f + BOARD_HEX_TILE_WIDTH;
    units->unit_positions[unit_index * 12 + 3] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f;
    units->unit_positions[unit_index * 12 + 4] = (float)x * BOARD_HEX_TILE_WIDTH * 0.75f;
    units->unit_positions[unit_index * 12 + 5] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f;
    units->unit_positions[unit_index * 12 + 6] = (float)x * BOARD_HEX_TILE_WIDTH * 0.75f;
    units->unit_positions[unit_index * 12 + 7] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f + BOARD_HEX_TILE_HEIGHT;
    units->unit_positions[unit_index * 12 + 8] = (float)x * BOARD_HEX_TILE_WIDTH * 0.75f + BOARD_HEX_TILE_WIDTH;
    units->unit_positions[unit_index * 12 + 9] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f;
    units->unit_positions[unit_index * 12 + 10] = (float)x * BOARD_HEX_TILE_WIDTH * 0.75f + BOARD_HEX_TILE_WIDTH;
    units->unit_positions[unit_index * 12 + 11] =
        (float)y * BOARD_HEX_TILE_HEIGHT + (float)(x % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f + BOARD_HEX_TILE_HEIGHT;

    unit_update_health_position(units, unit_index);

    units->unit_update_flags |= UNIT_UPDATE;
}

void unit_update_uv(Units *units, int unit_index)
{
    units->unit_uvs[unit_index * 12] = (float)units->unit_type[unit_index] / TOTAL_UNIT_TYPES;
    units->unit_uvs[unit_index * 12 + 1] = 1.0f;
    units->unit_uvs[unit_index * 12 + 2] = ((float)units->unit_type[unit_index] + 1.0f) / TOTAL_UNIT_TYPES;
    units->unit_uvs[unit_index * 12 + 3] = 0.0f;
    units->unit_uvs[unit_index * 12 + 4] = (float)units->unit_type[unit_index] / TOTAL_UNIT_TYPES;
    units->unit_uvs[unit_index * 12 + 5] = 0.0f;
    units->unit_uvs[unit_index * 12 + 6] = (float)units->unit_type[unit_index] / TOTAL_UNIT_TYPES;
    units->unit_uvs[unit_index * 12 + 7] = 1.0f;
    units->unit_uvs[unit_index * 12 + 8] = ((float)units->unit_type[unit_index] + 1.0f) / TOTAL_UNIT_TYPES;
    units->unit_uvs[unit_index * 12 + 9] = 0.0f;
    units->unit_uvs[unit_index * 12 + 10] = ((float)units->unit_type[unit_index] + 1.0f) / TOTAL_UNIT_TYPES;
    units->unit_uvs[unit_index * 12 + 11] = 1.0f;
    units->unit_update_flags |= UNIT_UPDATE;
}

void unit_update_color(Units *units, int unit_index)
{
    float color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    switch (units->unit_owner[unit_index])
    {
    case 1:
        color[0] = 0.7f;
        break;
    case 2:
        color[1] = 0.7f;
        break;
    case 3:
        color[1] = 0.7f;
        color[2] = 0.7f;
        break;
    case 4:
        color[0] = 0.7f;
        color[1] = 0.7f;
        break;
    default:
        break;
    }
    for (int j = 0; j < 6; j++)
    {
        units->unit_colors[unit_index * 24 + j * 4] = color[0];
        units->unit_colors[unit_index * 24 + j * 4 + 1] = color[1];
        units->unit_colors[unit_index * 24 + j * 4 + 2] = color[2];
        units->unit_colors[unit_index * 24 + j * 4 + 3] = color[3];
    }

    units->unit_update_flags |= UNIT_UPDATE;
}

void unit_update_health_position(Units *units, int unit_index)
{
    units->unit_health_positions[unit_index * 12] = units->unit_positions[unit_index * 12] + g_health_bar_offset_x;
    units->unit_health_positions[unit_index * 12 + 1] =
        units->unit_positions[unit_index * 12 + 3] + g_health_bar_offset_y + g_health_bar_size_y;
    units->unit_health_positions[unit_index * 12 + 2] =
        units->unit_positions[unit_index * 12] + g_health_bar_offset_x +
        g_health_bar_size_x * units->unit_health[unit_index] / unit_base_health[units->unit_type[unit_index]];
    units->unit_health_positions[unit_index * 12 + 3] =
        units->unit_positions[unit_index * 12 + 3] + g_health_bar_offset_y;
    units->unit_health_positions[unit_index * 12 + 4] = units->unit_positions[unit_index * 12] + g_health_bar_offset_x;
    units->unit_health_positions[unit_index * 12 + 5] =
        units->unit_positions[unit_index * 12 + 3] + g_health_bar_offset_y;
    units->unit_health_positions[unit_index * 12 + 6] = units->unit_positions[unit_index * 12] + g_health_bar_offset_x;
    units->unit_health_positions[unit_index * 12 + 7] =
        units->unit_positions[unit_index * 12 + 3] + g_health_bar_offset_y + g_health_bar_size_y;
    units->unit_health_positions[unit_index * 12 + 8] =
        units->unit_positions[unit_index * 12] + g_health_bar_offset_x +
        g_health_bar_size_x * units->unit_health[unit_index] / unit_base_health[units->unit_type[unit_index]];
    units->unit_health_positions[unit_index * 12 + 9] =
        units->unit_positions[unit_index * 12 + 3] + g_health_bar_offset_y;
    units->unit_health_positions[unit_index * 12 + 10] =
        units->unit_positions[unit_index * 12] + g_health_bar_offset_x +
        g_health_bar_size_x * units->unit_health[unit_index] / unit_base_health[units->unit_type[unit_index]];
    units->unit_health_positions[unit_index * 12 + 11] =
        units->unit_positions[unit_index * 12 + 3] + g_health_bar_offset_y + g_health_bar_size_y;

    units->unit_update_flags |= UNIT_UPDATE_HEALTH;
}

void unit_update_health_uv(Units *units, int unit_index)
{
    units->unit_health_uvs[unit_index * 12] = 0.0f;
    units->unit_health_uvs[unit_index * 12 + 1] = 1.0f;
    units->unit_health_uvs[unit_index * 12 + 2] = 1.0f / 4.0f;
    units->unit_health_uvs[unit_index * 12 + 3] = 0.0f;
    units->unit_health_uvs[unit_index * 12 + 4] = 0.0f;
    units->unit_health_uvs[unit_index * 12 + 5] = 0.0f;
    units->unit_health_uvs[unit_index * 12 + 6] = 0.0f;
    units->unit_health_uvs[unit_index * 12 + 7] = 1.0f;
    units->unit_health_uvs[unit_index * 12 + 8] = 1.0f / 4.0f;
    units->unit_health_uvs[unit_index * 12 + 9] = 0.0f;
    units->unit_health_uvs[unit_index * 12 + 10] = 1.0f / 4.0f;
    units->unit_health_uvs[unit_index * 12 + 11] = 1.0f;
    units->unit_update_flags |= UNIT_UPDATE_HEALTH;
}

void unit_update_health_color(Units *units, int unit_index)
{
    for (int j = 0; j < 6; j++)
    {
        units->unit_health_colors[unit_index * 24 + j * 4] = 1.0f;
        units->unit_health_colors[unit_index * 24 + j * 4 + 1] = 0.0f;
        units->unit_health_colors[unit_index * 24 + j * 4 + 2] = 1.0f;
        units->unit_health_colors[unit_index * 24 + j * 4 + 3] = 1.0f;
    }
    units->unit_update_flags |= UNIT_UPDATE_HEALTH;
}

void unit_occupy_new_tile(Units *units, int unit_index, int previous_x, int previous_y, int new_x, int new_y,
                          int board_dimension_x)
{
    if (units->unit_tile_occupation_status[previous_y * board_dimension_x + previous_x] == unit_index)
    {
        units->unit_tile_occupation_status[previous_y * board_dimension_x + previous_x] = -1;
    }
    units->unit_tile_occupation_status[new_y * board_dimension_x + new_x] = unit_index;
    units->unit_indices[unit_index * 2] = new_x;
    units->unit_indices[unit_index * 2 + 1] = new_y;
    int start_q, start_r, target_q, target_r;
    hex_grid_offset_to_axial(previous_x, previous_y, &start_q, &start_r);
    hex_grid_offset_to_axial(new_x, new_y, &target_q, &target_r);
    units->unit_movement_points[unit_index] -= (float)hex_grid_get_axial_distance(start_q, start_r, target_q, target_r);
}

void unit_add_movement_animation(Units *units, int unit_index, int start_x, int start_y, int end_x, int end_y,
                                 int move_type)
{
    da_int_push_back(&units->moves_unit_index, unit_index);
    da_int_push_back(&units->moves_list, start_x);
    da_int_push_back(&units->moves_list, start_y);
    da_int_push_back(&units->moves_list, end_x);
    da_int_push_back(&units->moves_list, end_y);
    da_int_push_back(&units->move_type, move_type);
}

bool unit_animate_movement(Units *units)
{
    int unit_index = units->moves_unit_index.array[0];
    float start_x = (float)units->moves_list.array[0] * BOARD_HEX_TILE_WIDTH * 0.75f;
    float start_y = (float)units->moves_list.array[1] * BOARD_HEX_TILE_HEIGHT +
                    (float)(units->moves_list.array[0] % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f;
    float end_x = (float)units->moves_list.array[2] * BOARD_HEX_TILE_WIDTH * 0.75f;
    float end_y = (float)units->moves_list.array[3] * BOARD_HEX_TILE_HEIGHT +
                  (float)(units->moves_list.array[2] % 2) * BOARD_HEX_TILE_HEIGHT / 2.0f;
    float new_x = glm_lerp(start_x, end_x, units->unit_animation_progress);
    float new_y = glm_lerp(start_y, end_y, units->unit_animation_progress);
    units->unit_positions[unit_index * 12] = new_x;
    units->unit_positions[unit_index * 12 + 1] = new_y + BOARD_HEX_TILE_HEIGHT;
    units->unit_positions[unit_index * 12 + 2] = new_x + BOARD_HEX_TILE_WIDTH;
    units->unit_positions[unit_index * 12 + 3] = new_y;
    units->unit_positions[unit_index * 12 + 4] = new_x;
    units->unit_positions[unit_index * 12 + 5] = new_y;
    units->unit_positions[unit_index * 12 + 6] = new_x;
    units->unit_positions[unit_index * 12 + 7] = new_y + BOARD_HEX_TILE_HEIGHT;
    units->unit_positions[unit_index * 12 + 8] = new_x + BOARD_HEX_TILE_WIDTH;
    units->unit_positions[unit_index * 12 + 9] = new_y;
    units->unit_positions[unit_index * 12 + 10] = new_x + BOARD_HEX_TILE_WIDTH;
    units->unit_positions[unit_index * 12 + 11] = new_y + BOARD_HEX_TILE_HEIGHT;

    unit_update_health_position(units, unit_index);

    units->unit_update_flags |= UNIT_UPDATE;

    if (units->unit_animation_progress >= 1.0f)
    {
        da_int_remove(&units->moves_unit_index, 0);
        da_int_remove(&units->moves_list, 0);
        da_int_remove(&units->moves_list, 0);
        da_int_remove(&units->moves_list, 0);
        da_int_remove(&units->moves_list, 0);
        da_int_remove(&units->move_type, 0);
        units->unit_animation_progress = 0.0f;
        return true;
    }
    // TODO: replace 0.05f with variable
    units->unit_animation_progress += 0.05f;
    return false;
}

void units_clear(Units *units)
{
    units->unit_buffer_size = 0;
    units->unit_update_flags = 0;
    units->unit_animation_progress = 0.0f;
    free(units->unit_owner);
    units->unit_owner = NULL;
    free(units->unit_positions);
    units->unit_positions = NULL;
    free(units->unit_uvs);
    units->unit_uvs = NULL;
    free(units->unit_colors);
    units->unit_colors = NULL;
    free(units->unit_health);
    units->unit_health = NULL;
    free(units->unit_health_positions);
    units->unit_health_positions = NULL;
    free(units->unit_health_uvs);
    units->unit_health_uvs = NULL;
    free(units->unit_health_colors);
    units->unit_health_colors = NULL;
    free(units->unit_tile_occupation_status);
    units->unit_tile_occupation_status = NULL;
    free(units->unit_tile_ownership_status);
    units->unit_tile_ownership_status = NULL;
    free(units->unit_indices);
    units->unit_indices = NULL;
    da_int_free(&units->unit_freed_indices);
    da_int_free(&units->moves_unit_index);
    da_int_free(&units->moves_list);
    da_int_free(&units->move_type);
}

void units_destroy(Units *units)
{
    units_clear(units);
    free(units);
}