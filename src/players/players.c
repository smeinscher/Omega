//
// Created by sterling on 8/3/24.
//

#include "players.h"
#include "../util/general_helpers.h"
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>

Players *players_create(int player_count, int human_players_count, int *human_players)
{
    Players *players = malloc(sizeof(Players));
    if (players == NULL)
    {
        printf("Failed to allocate players\n");
        return NULL;
    }

    players->player_count = player_count;
    players->player_flags = 0;
    players->human_players_count = human_players_count;
    if (human_players == NULL)
    {
        players->human_players = malloc(sizeof(int) * human_players_count);
        if (players->human_players == NULL)
        {
            printf("Failed to allocate human_players\n");
            free(players);
            return NULL;
        }
        for (int i = 0; i < human_players_count; i++)
        {
            players->human_players[i] = i;
        }
    }
    else
    {
        players->human_players = human_players;
    }
    players->player_score = NULL;
    players->player_color = NULL;
    players->desired_unit_purchase = NULL;
    players->player_unit_count = NULL;
    players->resources = NULL;

    players->player_score = malloc(sizeof(int) * player_count);
    if (players->player_score == NULL)
    {
        printf("Failed to allocate player_score\n");
        players_destroy(players);
        return NULL;
    }
    memset(players->player_score, 0, sizeof(int) * player_count);
    players->player_color = malloc(sizeof(float) * player_count * 4);
    if (players->player_color == NULL)
    {
        printf("Failed to allocate player_color\n");
        players_destroy(players);
        return NULL;
    }

    players->desired_unit_purchase = malloc(sizeof(int) * player_count);
    if (players->desired_unit_purchase == NULL)
    {
        printf("Failed to allocate desired_unit_purchase\n");
        players_destroy(players);
        return NULL;
    }
    for (int i = 0; i < player_count; i++)
    {
        int rand_unit = rand() % 2 + 1;
        players->desired_unit_purchase[i] = rand_unit;
    }

    players->player_unit_count = malloc(sizeof(int) * player_count);
    if (players->player_unit_count == NULL)
    {
        printf("Failed to allocate player_unit_count\n");
        players_destroy(players);
        return NULL;
    }
    for (int i = 0; i < player_count; i++)
    {
        players->player_unit_count[i] = 2;
    }

    players->resources = resources_create(player_count);
    if (players->resources == NULL)
    {
        return NULL;
    }

    return players;
}

void players_reset(Players **players)
{
    int player_count = (*players)->player_count;
    int human_players_count = (*players)->human_players_count;
    int *human_players = NULL;
    if ((*players)->human_players != NULL)
    {
        human_players = malloc(sizeof(int) * human_players_count);
        if (human_players != NULL)
        {
            for (int i = 0; i < human_players_count; i++)
            {
                human_players[i] = (*players)->human_players[i];
            }
        }
        else
        {
            printf("Failed to allocate human_players\n");
        }
    }
    players_destroy(*players);
    *players = players_create(player_count, human_players_count, human_players);
}

void player_add(Board *board, Players *players, float *player_color, bool is_human)
{
    printf("player_add not implemented\n");
}

void player_remove(Board *board, Players *players, int player_index)
{
    for (int i = 0; i < board->board_dimension_x * board->board_dimension_y; i++)
    {
        if (board->units->unit_tile_ownership_status[i] == player_index + 1)
        {
            board->units->unit_tile_ownership_status[i] = -1;
        }
    }
    printf("player_remove not implemented\n");
}

void player_start_turn(Board *board, Players *players, int player_index)
{
    // TODO: randomize, or base this on something the player does
    //    players->resources->radioactive_material_count[player_index] += 10;
    //    players->resources->rare_metals_count[player_index] += 100;
    //    players->resources->common_metals_count[player_index] += 1000;

    players->player_flags ^= PLAYER_ENDED_TURN;

    for (int i = 0; i < board->units->unit_buffer_size; i++)
    {
        if (board->units->unit_owner[i] == player_index + 1)
        {
            unit_replenish_movement(board->units, i);
            unit_replenish_health(board->units, i);
        }
    }

    if (!is_human_player(players, player_index))
    {
        for (int i = 0; i < board->units->unit_buffer_size; i++)
        {
            if (board->units->unit_owner[i] == player_index + 1)
            {
                if (board->units->unit_type[i] == STATION)
                {
                    DynamicIntArray *possible_moves = hex_grid_possible_moves(
                        board, i, board->units->unit_indices[i * 2], board->units->unit_indices[i * 2 + 1]);
                    if (possible_moves->used < 2)
                    {
                        continue;
                    }
                    int r = rand() % possible_moves->used / 2;
                    while (unit_purchase_with_score(board->units, player_index, &players->player_score[player_index],
                                                    players->desired_unit_purchase[player_index],
                                                    possible_moves->array[r * 2], possible_moves->array[r * 2 + 1],
                                                    board->board_dimension_x, board->board_dimension_y))
                    {
                        int rand_unit = rand() % 2000;
                        switch (player_index)
                        {
                        case 0:
                            rand_unit = 2;
                            break;
                        case 1:
                            rand_unit = 1;
                            break;
                        case 2:
                            rand_unit = rand_unit > 250 ? 2 : 1;
                            break;
                        case 3:
                            rand_unit = rand_unit > 250 ? 1 : 2;
                            break;
                        default:
                            rand_unit = 1;
                            break;
                        }
                        players->desired_unit_purchase[player_index] = rand_unit;
                        da_int_free(possible_moves);
                        free(possible_moves);
                        possible_moves = hex_grid_possible_moves(board, i, board->units->unit_indices[i * 2],
                                                                 board->units->unit_indices[i * 2 + 1]);
                        if (possible_moves->used < 2)
                        {
                            break;
                        }
                        r = rand() % possible_moves->used / 2;
                    }
                    da_int_free(possible_moves);
                    free(possible_moves);
                    continue;
                }
                if (board->units->unit_type[i] != WORKER)
                {
                    DynamicIntArray *possible_attacks = hex_grid_possible_attacks(
                        board, i, board->units->unit_indices[i * 2], board->units->unit_indices[i * 2 + 1]);
                    if (possible_attacks->used > 0)
                    {
                        int r = rand() % possible_attacks->used / 2;
                        int defending_unit_index =
                            board->units->unit_tile_occupation_status[possible_attacks->array[r * 2 + 1] *
                                                                          board->board_dimension_x +
                                                                      possible_attacks->array[r * 2]];
                        if (defending_unit_index != -1)
                        {
                            int defending_unit_owner = board->units->unit_owner[defending_unit_index] - 1;
                            BattleResult result = board_process_attack(board, defending_unit_index, i);
                            switch (result)
                            {
                            case NO_UNITS_DESTROYED:
                                break;
                            case DEFENDER_DESTROYED:
                                players->player_score[player_index] += 25;
                                break;
                            case ATTACKER_DESTROYED:
                                players->player_score[defending_unit_owner] += 15;
                                break;
                            case BOTH_DESTROYED:
                                players->player_score[player_index] += 10;
                                players->player_score[defending_unit_owner] += 10;
                                break;
                            }
                            continue;
                        }
                        printf("Error, defending_unit_index is -1\n");
                    }
                }
                DynamicIntArray *possible_moves = hex_grid_possible_moves(board, i, board->units->unit_indices[i * 2],
                                                                          board->units->unit_indices[i * 2 + 1]);
                if (possible_moves->used != 0)
                {
                    int r = rand() % possible_moves->used / 2;
                    unit_add_movement_animation(board->units, i, board->units->unit_indices[i * 2],
                                                board->units->unit_indices[i * 2 + 1], possible_moves->array[r * 2],
                                                possible_moves->array[r * 2 + 1], 0);
                    unit_occupy_new_tile(board->units, i, board->units->unit_indices[i * 2],
                                         board->units->unit_indices[i * 2 + 1], possible_moves->array[r * 2],
                                         possible_moves->array[r * 2 + 1], board->board_dimension_x);
                    /*
                                        unit_move(board->units, i, board->units->unit_indices[i * 2],
                       board->units->unit_indices[i * 2 + 1], possible_moves->array[r * 2], possible_moves->array[r * 2
                       + 1], board->board_dimension_x, board->board_dimension_y);
                    */
                }
                da_int_free(possible_moves);
                free(possible_moves);
            }
        }

        player_end_turn(board, players, player_index);
    }
}

void player_end_turn(Board *board, Players *players, int player_index)
{
    players->player_flags |= PLAYER_ENDED_TURN;
    players->player_unit_count[player_index] = 0;
    for (int i = 0; i < board->board_dimension_x * board->board_dimension_y; i++)
    {
        if (board->units->unit_tile_ownership_status[i] == player_index + 1)
        {
            if (board->units->unit_tile_occupation_status[i] != -1)
            {
                players->player_unit_count[player_index]++;
            }
            players->player_score[player_index]++;
        }
    }
    board_process_turn(board);
}

bool is_human_player(Players *players, int player_index)
{
    for (int i = 0; i < players->human_players_count; i++)
    {
        if (players->human_players[i] == player_index)
        {
            return true;
        }
    }
    return false;
}

void players_clear(Players *players)
{
    players->player_count = 0;
    players->human_players_count = 0;
    free(players->human_players);
    players->human_players = NULL;
    free(players->player_score);
    players->player_score = NULL;
    free(players->player_color);
    players->player_color = NULL;
    free(players->desired_unit_purchase);
    players->desired_unit_purchase = NULL;
    resources_destroy(players->resources);
    players->resources = NULL;
}

void players_destroy(Players *players)
{
    players_clear(players);
    free(players);
}
