//
// Created by sterling on 7/9/24.
//

#include "main_game_scene.h"
#include "../objects/board.h"
#include "../renderer/camera.h"
#include "../renderer/opengl_renderer.h"
#include "../renderer/opengl_shader.h"
#include "../renderer/opengl_texture.h"
#include "scene_state.h"
#include "../platform/platform.h"

#include <stdbool.h>

static Board *current_board = NULL;

static unsigned int board_outline_program = 0;
static unsigned int board_outline_id = 0;
static unsigned int board_borders_program = 0;
static unsigned int board_borders_id = 0;
static unsigned int board_texture = 0;

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
            board_update_hovered_tile(current_board, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f);
            // glfwSetWindowShouldClose(window, true);
            scene_set(MAIN_MENU);
        }
        break;
    }
    case GLFW_KEY_W: {
        if (action == GLFW_PRESS)
        {
            board_update_hovered_tile(current_board, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f);
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
            board_update_hovered_tile(current_board, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f);
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
            board_update_hovered_tile(current_board, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f);
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
            board_update_hovered_tile(current_board, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f);
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
    vec4 pos_nds;
    vec4 mouse_position = {(float)xpos, (float)ypos, 0.0f, 1.0f};
    glm_vec4_mul((float *)camera_get_projection(), mouse_position, pos_nds);
    float left = (float)camera_get_viewport_width() - (float)camera_get_viewport_width() / camera_get_zoom_ratio();
    float right = (float)camera_get_viewport_width() / camera_get_zoom_ratio();
    float bottom = (float)camera_get_viewport_height() / camera_get_zoom_ratio();
    float top = (float)camera_get_viewport_height() - (float)camera_get_viewport_height() / camera_get_zoom_ratio();
    float a = 100.0f / (100.0f - (100.0f - (camera_get_zoom_ratio() * 100.0f)) * 2.0f);
    float modified_x_pos = (pos_nds[0] + a) / (a * 2.0f) * (right - left);
    float modified_y_pos = (bottom - top) - (pos_nds[1] + a) / (a * 2.0f) * (bottom - top);

    board_update_hovered_tile(current_board, modified_x_pos + (*camera_get_position())[0],
                              modified_y_pos + (*camera_get_position())[1], camera_get_zoom_ratio(), right - left,
                              bottom - top);
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
    board_update_hovered_tile(current_board, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f);
    camera_decrement_zoom((float)yoffset);
    if (camera_get_zoom() < 55.0f)
    {
        camera_set_zoom(55.0f);
    }
    else if (camera_get_zoom() > 155.0f)
    {
        camera_set_zoom(155.0f);
    }
}

void main_game_scene_init()
{
    platform_set_callbacks(main_game_scene_glfw_framebuffer_size_callback, main_game_scene_glfw_key_callback,
                           main_game_scene_glfw_cursor_position_callback, main_game_scene_glfw_mouse_button_callback,
                           main_game_scene_glfw_scroll_callback);
    Board *board = board_create(21, 21);
    board_outline_program =
        basic_shader_create("resources/shaders/board_outline.vert", "resources/shaders/board_outline.frag");
    board_outline_id = basic_vertex_data_create(
        board->board_outline_vertices, 2, NULL, NULL, board->board_dimension_y * (board->board_dimension_x * 2 + 2) * 2,
        NULL, 0, board->board_outline_indices, board->board_dimension_x * board->board_dimension_y * 12, 0);
    board_borders_program =
        basic_shader_create("resources/shaders/board_borders.vert", "resources/shaders/board_borders.frag");
    board_borders_id = basic_vertex_data_create(board->board_border_positions, 2, board->board_border_uvs,
                                                board->board_border_colors, 6, board->board_tile_offsets,
                                                board->board_dimension_x * board->board_dimension_y, NULL, 0, 0);

    current_board = board;

    board_texture = generate_opengl_texture("resources/textures/hex.png");
}

void main_game_scene_update()
{
    camera_update();
}

void main_game_scene_render()
{
    glBindTexture(GL_TEXTURE_2D, board_texture);
    opengl_set_uniform_mat4(board_borders_program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(board_borders_program, "projection", (vec4 *)camera_get_projection());
    opengl_set_uniform_mat4(board_outline_program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(board_outline_program, "projection", (vec4 *)camera_get_projection());
    basic_draw_elements(board_outline_id, board_outline_program, GL_LINES);
    basic_draw_arrays_instanced(board_borders_id, board_borders_program, 21 * 21);
}

void main_game_scene_clean()
{
    clean_all_vertex_data();
    glDeleteProgram(board_borders_program);
    glDeleteProgram(board_outline_program);
    free(current_board);
}
