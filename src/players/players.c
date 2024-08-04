//
// Created by sterling on 8/3/24.
//

#include "players.h"
#include "../util/general_helpers.h"
#include <malloc.h>
#include <memory.h>

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

    if (!is_human_player(players, player_index))
    {
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
