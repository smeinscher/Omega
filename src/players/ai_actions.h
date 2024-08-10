//
// Created by sterling on 8/10/24.
//

#include "../objects/board.h"
#include "../util/dynamic_array.h"
#include "players.h"
#ifndef OMEGAAPP_AI_ACTIONS_H
#define OMEGAAPP_AI_ACTIONS_H

void ai_spawn_unit(Board *board, Players *players, int player_index, DynamicIntArray *station_indices);

bool ai_unit_attack(Board *board, Players *players, int player_index, int unit_index);

void ai_unit_move(Board *board, Players *players, int player_index, int unit_index);

#endif // OMEGAAPP_AI_ACTIONS_H
