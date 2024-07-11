//
// Created by sterling on 7/9/24.
//

#include "scene_state.h"
#include "main_game_scene.h"
#include "main_menu_scene.h"
#include "../platform/platform.h"

#include <stddef.h>
#include <stdio.h>

SceneState g_scene_state;

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

void scene_set(SceneState new_scene_state)
{
    platform_set_callbacks(NULL, NULL, NULL, NULL, NULL);
    switch (g_scene_state)
    {
    case INIT:
        break;
    case MAIN_MENU:
        main_menu_scene_clean();
        break;
    case MAIN_GAME:
        main_game_scene_clean();
        break;
    }
    switch (new_scene_state)
    {
    case INIT:
        printf("Why do you do this...?\n");
        break;
    case MAIN_MENU:
        main_menu_scene_init();
        g_update_func_ptr = main_menu_scene_update;
        g_render_func_ptr = main_menu_scene_render;
        break;
    case MAIN_GAME:
        main_game_scene_init();
        g_update_func_ptr = main_game_scene_update;
        g_render_func_ptr = main_game_scene_render;
        break;
    }
    g_scene_state = new_scene_state;
}
