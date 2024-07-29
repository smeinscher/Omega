//
// Created by sterling on 7/9/24.
//

#include "scene_state.h"
#include "../platform/platform.h"
#include "main_game_scene.h"
#include "main_menu_scene.h"

#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>

static SceneState g_scene_state;

void (*g_init_func_ptr)(void) = NULL;

void (*g_update_func_ptr)(void) = NULL;

void (*g_render_func_ptr)(void) = NULL;

bool g_needs_init = true;

SceneState get_current_scene()
{
    return g_scene_state;
}

void scene_init()
{
    g_init_func_ptr();
    g_needs_init = false;
}

void scene_update()
{
    g_update_func_ptr();
}

void scene_render()
{
    g_render_func_ptr();
}

void scene_refresh_func_ptrs(void *game_dl)
{
    switch (g_scene_state)
    {
    case INIT:
        printf("Why do you do this...?\n");
        break;
    case MAIN_MENU:
        g_update_func_ptr = dlsym(game_dl, "main_menu_scene_update");
        g_render_func_ptr = dlsym(game_dl, "main_menu_scene_render");
        break;
    case MAIN_GAME:
        g_update_func_ptr = main_game_scene_update;
        g_render_func_ptr = main_game_scene_render;
        break;
    }
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
        g_init_func_ptr = main_menu_scene_init;
        g_update_func_ptr = main_menu_scene_update;
        g_render_func_ptr = main_menu_scene_render;
        break;
    case MAIN_GAME:
        g_init_func_ptr = main_game_scene_init;
        g_update_func_ptr = main_game_scene_update;
        g_render_func_ptr = main_game_scene_render;
        break;
    }
    g_scene_state = new_scene_state;
    g_needs_init = true;
}

bool scene_needs_init()
{
    return g_needs_init;
}
