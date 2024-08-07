//
// Created by sterling on 8/3/24.
//

#ifndef OMEGAAPP_PLAYERS_H
#define OMEGAAPP_PLAYERS_H

#include "../objects/board.h"
#include "resources.h"
#include <stdbool.h>

#define PLAYER_ENDED_TURN 0x1

typedef struct Players
{
    int player_count;

    int player_flags;

    int human_players_count;
    int *human_players;

    int *player_score;
    float *player_color;

    int *desired_unit_purchase;

    int *player_unit_count;

    Resources *resources;
} Players;

Players *players_create(int player_count, int human_players_count, int *human_players);

void players_reset(Players **players);

void player_add(Board *board, Players *players, float *player_color, bool is_human);

void player_remove(Board *board, Players *players, int player_index);

void player_start_turn(Board *board, Players *players, int player_index);

void player_end_turn(Board *board, Players *players, int player_index);

bool is_human_player(Players *players, int player_index);

void players_clear(Players *players);

void players_destroy(Players *players);

#endif // OMEGAAPP_PLAYERS_H
