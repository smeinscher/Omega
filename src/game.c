//
// Created by sterling on 7/10/24.
//

#include "game.h"
#include "scenes/scene_state.h"
#include "ui/nuklear_config.h"
#include <stdio.h>

void game_init(GLFWwindow *main_window)
{
    omega_nuklear_init(main_window);
    omega_nuklear_set_style(OMEGA_NUKLEAR_DARK);
    scene_set(MAIN_MENU, main_window);
}

void game_update()
{
    scene_update();
}

void game_render()
{
    scene_render();
}

void game_shutdown()
{
    omega_nuklear_shutdown();
}
