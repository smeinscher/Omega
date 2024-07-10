//
// Created by sterling on 7/9/24.
//

#ifndef OMEGA_SCENE_STATE_H
#define OMEGA_SCENE_STATE_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

typedef enum SceneState
{
    MAIN_MENU,
    MAIN_GAME
} SceneState;

void scene_update();

void scene_render();

void scene_set(SceneState scene_state, GLFWwindow *main_window);

#endif // OMEGA_SCENE_STATE_H
