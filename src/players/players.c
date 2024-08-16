//
// Created by sterling on 8/3/24.
//

#include "players.h"
#include "../util/general_helpers.h"
#include "ai_actions.h"
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
        players->desired_unit_purchase[i] = 0;
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
            unit_replenish_health(board->units, i, 25.0f);
            if (board->units->unit_type[i] == WORKER)
            {
                int q = 0;
                int r = -1;
                for (int j = 0; j < 6; j++)
                {
                    int x, y;
                    hex_grid_axial_to_offset(q, r, board->units->unit_indices[i * 2 + 1], &x, &y);
                    x += board->board_dimension_x / 2;
                    y += board->board_dimension_y / 2;
                    int unit_index = board->units->unit_tile_occupation_status[y * board->board_dimension_x + x];
                    if (board->units->unit_owner[unit_index] == player_index + 1)
                    {
                        unit_replenish_health(board->units, unit_index, 50.0f);
                    }
                    hex_grid_rotation_get_next(false, 1, &q, &r);
                }
            }
        }
    }

    if (!is_human_player(players, player_index))
    {
        DynamicIntArray stations;
        da_int_init(&stations, 1);
        DynamicIntArray *moving_units = malloc(sizeof(DynamicIntArray));
        da_int_init(moving_units, 1);
        for (int i = 0; i < board->units->unit_buffer_size; i++)
        {
            if (board->units->unit_owner[i] == player_index + 1)
            {
                if (board->units->unit_type[i] == STATION)
                {
                    da_int_push_back(&stations, i);
                }
                else
                {
                    da_int_push_back(moving_units, i);
                }
            }
        }
        ai_start_turn(board, players, player_index, moving_units);
        ai_spawn_units(board, players, player_index, &stations);
        da_int_free(&stations);

        if (ai_process_next_move(board, players, player_index))
        {
            player_end_turn(board, players, player_index);
        }
        /*for (int i = 0; i < board->units->unit_buffer_size; i++)
        {
            if (board->units->unit_owner[i] != player_index + 1 || board->units->unit_type[i] == STATION)
            {
                continue;
            }
            if (board->units->unit_type[i] != WORKER)
            {
                if (ai_unit_attack(board, players, player_index, i))
                {
                    continue;
                }
            }
            ai_unit_move(board, players, player_index, i);
        }
        player_end_turn(board, players, player_index);
        */
    }
}

void player_end_turn(Board *board, Players *players, int player_index)
{
    players->player_flags |= PLAYER_ENDED_TURN;
    players->player_unit_count[player_index] = 0;
    if (!is_human_player(players, player_index))
    {
        players->player_score[player_index] += 100;
    }
    for (int i = 0; i < board->board_dimension_x * board->board_dimension_y; i++)
    {
        if (board->units->unit_tile_ownership_status[i] == player_index + 1)
        {
            players->player_score[player_index] += 10;
            int unit_index = board->units->unit_tile_occupation_status[i];
            if (unit_index == -1)
            {
                continue;
            }
            players->player_unit_count[player_index]++;
            if (board->units->unit_type[unit_index] == STATION)
            {
                if (planet_on_tile(board->planets, i % board->board_dimension_x, i / board->board_dimension_x))
                {
                    players->player_score[player_index] += 500;
                }
                else
                {
                    players->player_score[player_index] += 200;
                }
            }
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
    free(players->player_unit_count);
    players->player_unit_count = NULL;
    resources_destroy(players->resources);
    players->resources = NULL;
}

void players_destroy(Players *players)
{
    players_clear(players);
    free(players);
}
