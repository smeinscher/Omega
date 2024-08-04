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
    players->human_players_count = human_players_count;
    if (players->human_players == NULL)
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

    players->resources = resources_create(player_count);
    if (players->resources == NULL)
    {
        return NULL;
    }

    return players;
}

void player_add(Players *players, float *player_color, bool is_human)
{
    printf("player_add not implemented\n");
}

void player_remove(Players *players, int player_index)
{
    printf("player_remove not implemented\n");
}

void player_start_turn(Board *board, Players *players, int player_index)
{
    // TODO: randomize, or base this on something the player does
    players->resources->radioactive_material_count[player_index] += 10;
    players->resources->rare_metals_count[player_index] += 100;
    players->resources->common_metals_count[player_index] += 1000;

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
                            board_process_attack(board, defending_unit_index, i);
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
                    unit_move(board->units, i, board->units->unit_indices[i * 2], board->units->unit_indices[i * 2 + 1],
                              possible_moves->array[r * 2], possible_moves->array[r * 2 + 1], board->board_dimension_x,
                              board->board_dimension_y);
                }
                da_int_free(possible_moves);
                free(possible_moves);
            }
        }
        player_end_turn(board, players, player_index);
        player_index++;
        if (player_index >= players->player_count)
        {
            player_index = 0;
        }
        player_start_turn(board, players, player_index);
    }
}

void player_end_turn(Board *board, Players *players, int player_index)
{
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
    resources_destroy(players->resources);
    players->resources = NULL;
}

void players_destroy(Players *players)
{
    players_clear(players);
    free(players);
}
