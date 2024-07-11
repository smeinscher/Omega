//
// Created by sterling on 7/10/24.
//

#ifndef OMEGAAPP_GAME_H
#define OMEGAAPP_GAME_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void game_init(GLFWwindow *main_window);

void game_update();

void game_render();

void game_shutdown();

#endif // OMEGAAPP_GAME_H
