//
// Created by sterling on 7/9/24.
//

#ifndef OMEGA_SCENE_STATE_H
#define OMEGA_SCENE_STATE_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdbool.h>

typedef enum SceneState
{
    INIT,
    MAIN_MENU,
    MAIN_GAME
} SceneState;

SceneState get_current_scene();

void scene_init();

void scene_update();

void scene_render();

void scene_refresh_func_ptrs(void *game_dl);

void scene_set(SceneState new_scene_state);

bool scene_needs_init();

#endif // OMEGA_SCENE_STATE_H
