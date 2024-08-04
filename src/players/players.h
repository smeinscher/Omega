//
// Created by sterling on 8/3/24.
//

#ifndef OMEGAAPP_PLAYERS_H
#define OMEGAAPP_PLAYERS_H

#include "../objects/board.h"
#include "resources.h"
#include <stdbool.h>

typedef struct Players
{
    int player_count;

    int human_players_count;
    int *human_players;

    int *player_score;
    float *player_color;

    Resources *resources;
} Players;

Players *players_create(int player_count, int human_players_count, int *human_players);

void player_add(Players *players, float *player_color, bool is_human);

void player_remove(Players *players, int player_index);

void player_start_turn(Board *board, Players *players, int player_index);

void player_end_turn(Board *board, Players *players, int player_index);

bool is_human_player(Players *players, int player_index);

void players_clear(Players *players);

void players_destroy(Players *players);

#endif // OMEGAAPP_PLAYERS_H
