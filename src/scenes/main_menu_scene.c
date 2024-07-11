//
// Created by sterling on 7/9/24.
//

#include "main_menu_scene.h"
#include "../../external/nuklear/nuklear.h"
#include "../ui/nuklear_config.h"
#include "scene_state.h"

static GLFWwindow *g_main_window = NULL;

void main_menu_scene_init(GLFWwindow *main_window)
{
    g_main_window = main_window;
}

void main_menu_scene_update()
{
}

void main_menu_scene_render()
{
    struct nk_context *ctx = omega_nuklear_get_nuklear_context();
    omega_nuklear_start();
    if (nk_begin(ctx, "Demo", nk_rect(540, 330, 210, 70), NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_static(ctx, 60, 200, 1);
        if (nk_button_label(ctx, "New Game"))
        {
            scene_set(MAIN_GAME, g_main_window);
        }
    }
    omega_nuklear_end();
}

void main_menu_scene_clean()
{
}
