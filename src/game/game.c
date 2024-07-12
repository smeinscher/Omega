//
// Created by sterling on 7/10/24.
//

#include "game.h"

#include "../platform/platform.h"
#include "../renderer/opengl_renderer.h"
#include "../scenes/scene_state.h"
#include "../ui/nuklear_config.h"

#include <stdio.h>

#define TIME_PER_UPDATE (1.0 / 60.0)
static double previous_update_time = 0.0;
static double lag = 0.0;

int game_init(int width, int height, const char *title)
{
    platform_init();
    if (platform_create_window(width, height, title))
    {
        // TODO: logging stuff
        printf("Error creating window\n");
        return -1;
    }
    if (opengl_glad_init())
    {
        // TODO: logging stuff
        printf("Error initializing glad\n");
        return -1;
    }
    opengl_enable_default_attributes();
    platform_set_vsync(1);
    platform_init_nuklear();
    scene_set(MAIN_MENU);
    return 0;
}

void game_reload(void *game_dl)
{
    scene_refresh_func_ptrs(game_dl);
}

void game_update()
{
    platform_poll_events();
    double current_update_time = platform_get_time();
    double elapsed_time = current_update_time - previous_update_time;
    previous_update_time = current_update_time;
    lag += elapsed_time;
    while (lag >= TIME_PER_UPDATE)
    {
        scene_update();
        lag -= TIME_PER_UPDATE;
    }
}

void game_render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    scene_render();
    platform_swap_buffers();
}

void game_shutdown()
{
    omega_nuklear_shutdown();
    platform_destroy_window();
    platform_clean();
}

bool game_is_running()
{
    return !platform_window_should_closs();
}
