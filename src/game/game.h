//
// Created by sterling on 7/10/24.
//

#ifndef OMEGAAPP_GAME_H
#define OMEGAAPP_GAME_H
#include <stdbool.h>

int game_init(int width, int height, const char *title);

void game_update();

void game_render();

void game_shutdown();

bool game_is_running();

#endif // OMEGAAPP_GAME_H
