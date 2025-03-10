//

#include "main_menu_scene.h"
#include "../../external/nuklear/nuklear.h"
#include "../platform/platform.h"
#include "../ui/nuklear_config.h"
#include "scene_state.h"
#include <stdio.h>

void main_menu_scene_glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE)
    {
        if (action == GLFW_PRESS)
        {
            platform_set_window_should_close(true);
        }
    }
}

void main_menu_scene_init()
{
    platform_set_callbacks(NULL, main_menu_scene_glfw_key_callback, NULL, NULL, NULL);
}

void main_menu_scene_update()
{
}

void main_menu_scene_render()
{
    struct nk_context *ctx = omega_nuklear_get_nuklear_context();
    omega_nuklear_start();
    float x = (float)platform_get_window_width() / 2.0f - 105.0f;
    float y = (float)platform_get_window_height() / 2.0f - 35.0f;
    if (nk_begin(ctx, "Demo", nk_rect(x, y, 210.0f, 70.0f), NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_static(ctx, 60, 200, 1);
        if (nk_button_label(ctx, "New Game"))
        {
            scene_set(MAIN_GAME);
        }
    }
    nk_end(ctx);
    omega_nuklear_end();
}

void main_menu_scene_clean()
{
}
