//
// Created by sterling on 8/10/24.
//

#include "ai_actions.h"
#include "../util/general_helpers.h"
#include <stdlib.h>

static DynamicIntArray *g_current_moving_units;

void ai_start_turn(Board *board, Players *players, int player_index, DynamicIntArray *moving_units)
{
    g_current_moving_units = moving_units;
}

void ai_end_turn()
{
    da_int_free(g_current_moving_units);
    free(g_current_moving_units);
}

bool ai_process_next_move(Board *board, Players *players, int player_index)
{
    int index = g_current_moving_units->used - 1;
    if (index < 0)
    {
        ai_end_turn();
        return true;
    }
    if (!ai_unit_attack(board, players, player_index, g_current_moving_units->array[index]))
    {
        ai_unit_move(board, players, player_index, g_current_moving_units->array[index]);
    }
    da_int_pop_back(g_current_moving_units);

    return false;
}

void ai_spawn_units(Board *board, Players *players, int player_index, DynamicIntArray *station_indices)
{
    for (int i = 0; i < station_indices->used; i++)
    {
        int station_index = station_indices->array[i];
        DynamicIntArray *possible_moves =
            hex_grid_possible_moves(board, station_index, board->units->unit_indices[station_index * 2],
                                    board->units->unit_indices[station_index * 2 + 1]);
        if (possible_moves->used < 2)
        {
            continue;
        }
        int r = rand() % possible_moves->used / 2;
        int units_to_spawn = rand() % 6 + 1;
        int units_spawned = 0;
        while (units_spawned < units_to_spawn)
        {
            int unit_index = unit_purchase_with_score(board->units, player_index, &players->player_score[player_index],
                                                      players->desired_unit_purchase[player_index],
                                                      possible_moves->array[r * 2], possible_moves->array[r * 2 + 1],
                                                      board->board_dimension_x, board->board_dimension_y);
            if (unit_index == -1)
            {
                da_int_free(possible_moves);
                free(possible_moves);
                return;
            }
            da_int_push_back(g_current_moving_units, unit_index);
            int rand_unit = rand() % 3000;
            switch (player_index)
            {
            case 0:
                rand_unit = rand_unit > 300 ? DROID : WORKER;
                break;
            case 1:
                rand_unit = rand_unit > 300 ? BATTLESHIP : WORKER;
                break;
            case 2:
                rand_unit = rand_unit > 1000 ? DROID : rand_unit > 300 ? BATTLESHIP : WORKER;
                break;
            case 3:
                rand_unit = rand_unit > 1000 ? BATTLESHIP : rand_unit > 300 ? DROID : WORKER;
                break;
            default:
                rand_unit = 2;
                break;
            }
            players->desired_unit_purchase[player_index] = rand_unit;
            da_int_free(possible_moves);
            free(possible_moves);
            possible_moves =
                hex_grid_possible_moves(board, station_index, board->units->unit_indices[station_index * 2],
                                        board->units->unit_indices[station_index * 2 + 1]);
            if (possible_moves->used < 2)
            {
                break;
            }
            r = rand() % possible_moves->used / 2;
            units_spawned++;
        }
        da_int_free(possible_moves);
        free(possible_moves);
    }
}

bool ai_unit_attack(Board *board, Players *players, int player_index, int unit_index)
{
    if (board->units->unit_type[unit_index] == WORKER || board->units->unit_type[unit_index] == STATION)
    {
        return false;
    }
    DynamicIntArray *possible_attacks = hex_grid_possible_attacks(
        board, unit_index, board->units->unit_indices[unit_index * 2], board->units->unit_indices[unit_index * 2 + 1]);
    if (possible_attacks->used > 0)
    {
        int r = rand() % possible_attacks->used / 2;
        int defending_unit_index =
            board->units->unit_tile_occupation_status[possible_attacks->array[r * 2 + 1] * board->board_dimension_x +
                                                      possible_attacks->array[r * 2]];
        if (defending_unit_index != -1 && !unit_in_remove_list(board->units, defending_unit_index))
        {
            int start_x = board->units->unit_indices[unit_index * 2];
            int start_y = board->units->unit_indices[unit_index * 2 + 1];
            int end_x = board->units->unit_indices[defending_unit_index * 2];
            int end_y = board->units->unit_indices[defending_unit_index * 2 + 1];
            unit_add_movement_animation(board->units, unit_index, start_x, start_y, end_x, end_y, ATTACK);
            /*
                        int defending_unit_owner = board->units->unit_owner[defending_unit_index] - 1;
                        BattleResult result = board_process_attack(board, defending_unit_index, unit_index);
                        switch (result)
                        {
                        // TODO: change score update to reflect unit type
                        case NO_UNITS_DESTROYED:
                            break;
                        case DEFENDER_DESTROYED:
                            players->player_score[player_index] +=
                                unit_get_base_cost(board->units->unit_type[defending_unit_index]) / 8;
                            break;
                        case ATTACKER_DESTROYED:
                            players->player_score[defending_unit_owner] +=
                                unit_get_base_cost(board->units->unit_type[unit_index]) / 10;
                            break;
                        case BOTH_DESTROYED:
                            break;
                        }
            */
        }
        else
        {
            printf("Error, defending_unit_index is -1\n");
        }
        da_int_free(possible_attacks);
        free(possible_attacks);
        return true;
    }
    da_int_free(possible_attacks);
    free(possible_attacks);
    return false;
}

// TODO: clean this mess up
void ai_unit_move(Board *board, Players *players, int player_index, int unit_index)
{
    // TODO: cache these results?
    DynamicIntArray *desired_path = NULL;
    int unit_x = board->units->unit_indices[unit_index * 2];
    int unit_y = board->units->unit_indices[unit_index * 2 + 1];
    if (board->units->unit_type[unit_index] == WORKER)
    {
        // Find nearest friend
        int nearest_friend = units_find_nearest_friendly(board->units, player_index, unit_x, unit_y,
                                                         max_int(board->board_dimension_x, board->board_dimension_y));
        desired_path = hex_grid_find_path(board, unit_x, unit_y, board->units->unit_indices[nearest_friend * 2],
                                          board->units->unit_indices[nearest_friend * 2 + 1], board->board_dimension_x,
                                          board->board_dimension_y);
    }
    else
    {
        int nearest_enemy = units_find_nearest_enemy(board->units, player_index, unit_x, unit_y,
                                                     max_int(board->board_dimension_x, board->board_dimension_y));
        if (nearest_enemy != -1)
        {
            desired_path = hex_grid_find_path(board, unit_x, unit_y, board->units->unit_indices[nearest_enemy * 2],
                                              board->units->unit_indices[nearest_enemy * 2 + 1],
                                              board->board_dimension_x, board->board_dimension_y);
        }
    }
    DynamicIntArray *possible_moves = hex_grid_possible_moves(
        board, unit_index, board->units->unit_indices[unit_index * 2], board->units->unit_indices[unit_index * 2 + 1]);
    if (possible_moves->used != 0)
    {
        int move = -1;
        if (board->units->unit_type[unit_index] == WORKER)
        {
            if (unit_doing_action(board->units, unit_index, UNIT_BUILD_STATION))
            {
                unit_add_movement_animation(board->units, unit_index, -1, -1, -1, -1, NOTHING);
                return;
            }
            int planet_index = planet_find_closest(board->planets, board->units->unit_indices[unit_index * 2],
                                                   board->units->unit_indices[unit_index * 2 + 1]);
            int planet_x = board->planets->planet_tile_indices[planet_index * 2];
            int planet_y = board->planets->planet_tile_indices[planet_index * 2 + 1];
            if (planet_x == board->units->unit_indices[unit_index * 2] &&
                planet_y == board->units->unit_indices[unit_index * 2 + 1])
            {
                board_worker_build_station(board, unit_index);
                unit_add_movement_animation(board->units, unit_index, -1, -1, -1, -1, NOTHING);
                return;
            }
            if (board->units->unit_tile_occupation_status[planet_y * board->board_dimension_x + planet_x] == -1)
            {
                DynamicIntArray *path = hex_grid_find_path(board, board->units->unit_indices[unit_index * 2],
                                                           board->units->unit_indices[unit_index * 2 + 1],
                                                           board->planets->planet_tile_indices[planet_index * 2],
                                                           board->planets->planet_tile_indices[planet_index * 2 + 1],
                                                           board->board_dimension_x, board->board_dimension_y);
                if (path != NULL)
                {
                    for (int j = path->used - 2; j >= 0; j -= 2)
                    {
                        for (int k = 0; k < possible_moves->used; k += 2)
                        {
                            int x = possible_moves->array[k];
                            int y = possible_moves->array[k + 1];
                            if (x == path->array[j] && y == path->array[j + 1])
                            {
                                move = k / 2;
                                break;
                            }
                        }
                    }
                    da_int_free(path);
                    free(path);
                }
            }
            else
            {
                if (desired_path != NULL)
                {
                    for (int j = desired_path->used - 2; j >= 0; j -= 2)
                    {
                        for (int k = 0; k < possible_moves->used; k += 2)
                        {
                            int x = possible_moves->array[k];
                            int y = possible_moves->array[k + 1];
                            if (x == desired_path->array[j] && y == desired_path->array[j + 1])
                            {
                                move = k / 2;
                                break;
                            }
                        }
                    }
                    da_int_free(desired_path);
                    free(desired_path);
                    desired_path = NULL;
                }
            }
        }
        else if (desired_path != NULL)
        {
            for (int j = desired_path->used - 2; j >= 0; j -= 2)
            {
                for (int k = 0; k < possible_moves->used; k += 2)
                {
                    int x = possible_moves->array[k];
                    int y = possible_moves->array[k + 1];
                    if (x == desired_path->array[j] && y == desired_path->array[j + 1])
                    {
                        move = k / 2;
                        break;
                    }
                }
            }
            da_int_free(desired_path);
            free(desired_path);
            desired_path = NULL;
        }
        if (move == -1)
        {
            move = rand() % possible_moves->used / 2;
        }
        unit_add_movement_animation(board->units, unit_index, board->units->unit_indices[unit_index * 2],
                                    board->units->unit_indices[unit_index * 2 + 1], possible_moves->array[move * 2],
                                    possible_moves->array[move * 2 + 1], 0);
        //        unit_occupy_new_tile(board->units, unit_index, board->units->unit_indices[unit_index * 2],
        //                             board->units->unit_indices[unit_index * 2 + 1], possible_moves->array[move * 2],
        //                             possible_moves->array[move * 2 + 1], board->board_dimension_x);
        /*
                            unit_move(board->units, i, board->units->unit_indices[i * 2],
           board->units->unit_indices[i * 2 + 1], possible_moves->array[r * 2], possible_moves->array[r * 2
           + 1], board->board_dimension_x, board->board_dimension_y);
        */
    }
    da_int_free(desired_path);
    free(desired_path);
    da_int_free(possible_moves);
    free(possible_moves);
}
