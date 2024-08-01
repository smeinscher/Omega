//
// Created by sterling on 7/29/24.
//

#include "units.h"
#include "../util/general_helpers.h"
#include "board.h"
#include <memory.h>

static float g_health_bar_size_x = BOARD_HEX_TILE_WIDTH / 2.0f;
static float g_health_bar_size_y = BOARD_HEX_TILE_HEIGHT / 12.0f;
static float g_health_bar_offset_x = BOARD_HEX_TILE_WIDTH / 4.0f;
static float g_health_bar_offset_y = BOARD_HEX_TILE_HEIGHT / 2.0f + (BOARD_HEX_TILE_HEIGHT / 12.0f);

Units *units_create(int board_dimension_x, int board_dimension_y)
{
    Units *units = malloc(sizeof(Units));
    if (units == NULL)
    {
        printf("Error allocating units\n");
        return NULL;
    }

    units->unit_buffer_size = 0;
    units->unit_owner = NULL;
    units->unit_health = NULL;
    units->unit_positions = NULL;
    units->unit_uvs = NULL;
    units->unit_colors = NULL;
    units->unit_health_positions = NULL;
    units->unit_health_uvs = NULL;
    units->unit_health_colors = NULL;
    units->unit_tile_occupation_status = NULL;
    units->unit_movement_points = NULL;

    da_int_init(&units->unit_freed_indices, 4);

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

    units->unit_movement_points = malloc(sizeof(float));
    if (units->unit_movement_points == NULL)
    {
        printf("Error allocating unit_movement_points\n");
        units_destroy(units);
        return NULL;
    }
    units->unit_movement_points[0] = 2.0f;

    unit_add(units, 1, 1, 0, board_dimension_x);
    unit_add(units, 2, 1, board_dimension_y - 1, board_dimension_x);
    unit_add(units, 3, board_dimension_x - 2, 0, board_dimension_x);
    unit_add(units, 4, board_dimension_x - 2, board_dimension_y - 1, board_dimension_x);

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

void unit_add(Units *units, int owner, int x, int y, int board_dimension_x)
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
    units->unit_health[new_unit_index] = 100.0f;
    // TODO: replace 2.0f with something else
    units->unit_movement_points[new_unit_index] = 2.0f;

    unit_update_position(units, new_unit_index, x, y);

    unit_update_health_position(units, new_unit_index);

    unit_update_uv(units, new_unit_index);

    unit_update_health_uv(units, new_unit_index);

    unit_update_color(units, new_unit_index);

    unit_update_health_color(units, new_unit_index);
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

    units->unit_update_flags |= UNIT_UPDATE | UNIT_UPDATE_HEALTH;
}

bool unit_attack(Units *units, int unit_index, int x, int y, int board_dimension_x)
{
    units->unit_health[unit_index] -= 20.0f;
    if (units->unit_health[unit_index] > 0.0f)
    {
        unit_update_health_position(units, unit_index);
        return false;
    }

    unit_remove(units, unit_index, x, y, board_dimension_x);

    return true;
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
    // TODO: replace 4.0f with variable
    units->unit_uvs[unit_index * 12] = 2.0f / 4.0f;
    units->unit_uvs[unit_index * 12 + 1] = 1.0f;
    units->unit_uvs[unit_index * 12 + 2] = 3.0f / 4.0f;
    units->unit_uvs[unit_index * 12 + 3] = 0.0f;
    units->unit_uvs[unit_index * 12 + 4] = 2.0f / 4.0f;
    units->unit_uvs[unit_index * 12 + 5] = 0.0f;
    units->unit_uvs[unit_index * 12 + 6] = 2.0f / 4.0f;
    units->unit_uvs[unit_index * 12 + 7] = 1.0f;
    units->unit_uvs[unit_index * 12 + 8] = 3.0f / 4.0f;
    units->unit_uvs[unit_index * 12 + 9] = 0.0f;
    units->unit_uvs[unit_index * 12 + 10] = 3.0f / 4.0f;
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
    units->unit_health_positions[unit_index * 12 + 2] = units->unit_positions[unit_index * 12] + g_health_bar_offset_x +
                                                        g_health_bar_size_x * units->unit_health[unit_index] / 100.0f;
    units->unit_health_positions[unit_index * 12 + 3] =
        units->unit_positions[unit_index * 12 + 3] + g_health_bar_offset_y;
    units->unit_health_positions[unit_index * 12 + 4] = units->unit_positions[unit_index * 12] + g_health_bar_offset_x;
    units->unit_health_positions[unit_index * 12 + 5] =
        units->unit_positions[unit_index * 12 + 3] + g_health_bar_offset_y;
    units->unit_health_positions[unit_index * 12 + 6] = units->unit_positions[unit_index * 12] + g_health_bar_offset_x;
    units->unit_health_positions[unit_index * 12 + 7] =
        units->unit_positions[unit_index * 12 + 3] + g_health_bar_offset_y + g_health_bar_size_y;
    units->unit_health_positions[unit_index * 12 + 8] = units->unit_positions[unit_index * 12] + g_health_bar_offset_x +
                                                        g_health_bar_size_x * units->unit_health[unit_index] / 100.0f;
    units->unit_health_positions[unit_index * 12 + 9] =
        units->unit_positions[unit_index * 12 + 3] + g_health_bar_offset_y;
    units->unit_health_positions[unit_index * 12 + 10] = units->unit_positions[unit_index * 12] +
                                                         g_health_bar_offset_x +
                                                         g_health_bar_size_x * units->unit_health[unit_index] / 100.0f;
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

void units_clear(Units *units)
{
    units->unit_buffer_size = 0;
    units->unit_update_flags = 0;
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
    da_int_free(&units->unit_freed_indices);
}

void units_destroy(Units *units)
{
    units_clear(units);
    free(units);
}