//
// Created by sterling on 7/9/24.
//

#include "scene_state.h"
#include "main_game_scene.h"
#include "main_menu_scene.h"

#include <stddef.h>

SceneState g_scene_state = MAIN_GAME;

void (*g_update_func_ptr)(void) = NULL;

void (*g_render_func_ptr)(void) = NULL;

void scene_update()
{
    g_update_func_ptr();
}

void scene_render()
{
    g_render_func_ptr();
}

void scene_set(SceneState scene_state, GLFWwindow *main_window)
{
    switch (g_scene_state)
    {
    case MAIN_MENU:
        main_menu_scene_clean();
        break;
    case MAIN_GAME:
        main_game_scene_clean();
        break;
    }
    switch (scene_state)
    {
    case MAIN_MENU:
        main_menu_scene_init(main_window);
        g_update_func_ptr = main_menu_scene_update;
        g_render_func_ptr = main_menu_scene_render;
        break;
    case MAIN_GAME:
        main_game_scene_init(main_window);
        g_update_func_ptr = main_game_scene_update;
        g_render_func_ptr = main_game_scene_render;
        break;
    }
    g_scene_state = scene_state;
}
