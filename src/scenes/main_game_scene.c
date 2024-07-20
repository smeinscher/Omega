//
// Created by sterling on 7/9/24.
//

#include "main_game_scene.h"
#include "../../external/nuklear/nuklear.h"
#include "../objects/board.h"
#include "../platform/platform.h"
#include "../renderer/camera.h"
#include "../renderer/opengl_renderer.h"
#include "../renderer/opengl_shader.h"
#include "../renderer/opengl_texture.h"
#include "../ui/nuklear_config.h"
#include "scene_state.h"

#include <stdbool.h>

static Board *current_board = NULL;

static Shader board_outline_program = {0};
static unsigned int board_outline_id = 0;
static Shader board_borders_program = {0};
static unsigned int board_borders_id = 0;
static Shader board_fill_program = {0};
static unsigned int board_fill_id = 0;
static Texture board_texture = {0};

double g_mouse_pos_x = 0.0;
double g_mouse_pos_y = 0.0;

void main_game_scene_glfw_framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void main_game_scene_glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    switch (key)
    {
    case GLFW_KEY_ESCAPE: {
        if (action == GLFW_PRESS)
        {
            board_update_hovered_tile(current_board, -1.0f, -1.0f);
            // glfwSetWindowShouldClose(window, true);
            scene_set(MAIN_MENU);
        }
        break;
    }
    case GLFW_KEY_W: {
        if (action == GLFW_PRESS)
        {
            board_update_hovered_tile(current_board, -1.0f, -1.0f);
            camera_set_moving_up(true);
        }
        else if (action == GLFW_RELEASE)
        {
            camera_set_moving_up(false);
        }
        break;
    }
    case GLFW_KEY_S: {
        if (action == GLFW_PRESS)
        {
            board_update_hovered_tile(current_board, -1.0f, -1.0f);
            camera_set_moving_down(true);
        }
        else if (action == GLFW_RELEASE)
        {
            camera_set_moving_down(false);
        }
        break;
    }
    case GLFW_KEY_A: {
        if (action == GLFW_PRESS)
        {
            board_update_hovered_tile(current_board, -1.0f, -1.0f);
            camera_set_moving_left(true);
        }
        else if (action == GLFW_RELEASE)
        {
            camera_set_moving_left(false);
        }
        break;
    }
    case GLFW_KEY_D: {
        if (action == GLFW_PRESS)
        {
            board_update_hovered_tile(current_board, -1.0f, -1.0f);
            camera_set_moving_right(true);
        }
        else if (action == GLFW_RELEASE)
        {
            camera_set_moving_right(false);
        }
        break;
    }
    default:
        break;
    }
}

void main_game_scene_glfw_cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    g_mouse_pos_x = xpos;
    g_mouse_pos_y = ypos;
}

void main_game_scene_glfw_mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        board_handle_tile_click(current_board);
    }
}

void main_game_scene_glfw_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera_decrement_zoom((float)yoffset);
    if (camera_get_zoom() < 55.0f)
    {
        camera_set_zoom(55.0f);
    }
    else if (camera_get_zoom() > 125.0f)
    {
        camera_set_zoom(125.0f);
    }
}

void main_game_scene_update_hovered_tile()
{
    vec4 pos_nds;
    vec4 mouse_position = {(float)g_mouse_pos_x, (float)g_mouse_pos_y, 0.0f, 1.0f};
    glm_mat4_mulv((vec4 *)camera_get_projection(), mouse_position, pos_nds);
    float left = (float)camera_get_viewport_width() - (float)camera_get_viewport_width() / camera_get_zoom_ratio();
    float right = (float)camera_get_viewport_width() / camera_get_zoom_ratio();
    float bottom = (float)camera_get_viewport_height() / camera_get_zoom_ratio();
    float top = (float)camera_get_viewport_height() - (float)camera_get_viewport_height() / camera_get_zoom_ratio();
    float factor = camera_get_zoom_factor();
    float a = factor / (factor - (factor - camera_get_zoom()) * 2.0f);
    float modified_x_pos = (pos_nds[0] + a) / (a * 2.0f) * (right - left);
    float modified_y_pos = (bottom - top) - (pos_nds[1] + a) / (a * 2.0f) * (bottom - top);
    board_update_hovered_tile(current_board, modified_x_pos + (*camera_get_position())[0] + left,
                              modified_y_pos + (*camera_get_position())[1] + top);
}

void main_game_scene_init()
{
    platform_set_callbacks(main_game_scene_glfw_framebuffer_size_callback, main_game_scene_glfw_key_callback,
                           main_game_scene_glfw_cursor_position_callback, main_game_scene_glfw_mouse_button_callback,
                           main_game_scene_glfw_scroll_callback);
    Board *board = board_create(21, 21);
    board_outline_program =
        opengl_load_basic_shaders("../resources/shaders/board_outline.vert", "../resources/shaders/board_outline.frag");
    board_outline_id = basic_vertex_data_create(
        board->board_outline_vertices, 2, NULL, NULL, board->board_dimension_y * (board->board_dimension_x * 2 + 2) * 2,
        NULL, 0, board->board_outline_indices, board->board_dimension_x * board->board_dimension_y * 12, 0);

    board_borders_program =
        opengl_load_basic_shaders("../resources/shaders/board_borders.vert", "../resources/shaders/board_borders.frag");
    board_borders_id = basic_vertex_data_create(board->board_border_positions, 2, board->board_border_uvs,
                                                board->board_border_colors, 6, board->board_tile_offsets,
                                                board->board_dimension_x * board->board_dimension_y, NULL, 0, 0);

    board_fill_program =
        opengl_load_basic_shaders("../resources/shaders/board_fill.vert", "../resources/shaders/board_fill.frag");
    board_fill_id =
        basic_vertex_data_create(board->board_fill_positions, 2, NULL, board->board_fill_colors,
                                 board->board_dimension_y * board->board_dimension_x * 12, NULL, 0, NULL, 0, 0);

    current_board = board;

    board_texture = generate_opengl_texture("../resources/textures/hex.png");
}

void main_game_scene_update()
{
    camera_update();
    main_game_scene_update_hovered_tile();
}

void main_game_scene_render()
{
    opengl_texture_hot_reload(&board_texture);
    opengl_shader_hot_reload(&board_outline_program);
    opengl_shader_hot_reload(&board_borders_program);
    opengl_shader_hot_reload(&board_fill_program);
    glBindTexture(GL_TEXTURE_2D, board_texture.id);
    opengl_set_uniform_mat4(board_borders_program.program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(board_borders_program.program, "projection", (vec4 *)camera_get_projection());
    opengl_set_uniform_mat4(board_outline_program.program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(board_outline_program.program, "projection", (vec4 *)camera_get_projection());
    opengl_set_uniform_mat4(board_fill_program.program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(board_fill_program.program, "projection", (vec4 *)camera_get_projection());
    board_update_fill_vertices(current_board);
    basic_update_vertex_data(board_fill_id, current_board->board_fill_positions, NULL, current_board->board_fill_colors,
                             12 * current_board->board_dimension_x * current_board->board_dimension_y);
    basic_draw_elements(board_outline_id, board_outline_program.program, GL_LINES);
    basic_draw_arrays_instanced(board_borders_id, board_borders_program.program, 21 * 21);
    basic_draw_arrays(board_fill_id, board_fill_program.program, GL_TRIANGLES);
    omega_nuklear_start();
    struct nk_context *ctx = omega_nuklear_get_nuklear_context();
    if (nk_begin(ctx, "Debug", nk_rect(50, 50, 350, 250),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
    {
        nk_layout_row_static(ctx, 60, 350, 1);
        char mouse_pos_str[100];
        sprintf(mouse_pos_str, "Mouse Position: %.2lf, %.2lf", g_mouse_pos_x, g_mouse_pos_y);
        nk_label(ctx, mouse_pos_str, NK_TEXT_ALIGN_LEFT);
    }
    omega_nuklear_end();
}

void main_game_scene_clean()
{
    clean_all_vertex_data();
    glDeleteProgram(board_borders_program.program);
    glDeleteProgram(board_outline_program.program);
    glDeleteProgram(board_fill_program.program);
    board_destroy(current_board);
    current_board = NULL;
}
