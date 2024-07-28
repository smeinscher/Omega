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
#include "../util/file_helpers.h"
#include "scene_state.h"

#include <stdbool.h>

static Board *current_board = NULL;

static Shader board_outline_program = {0};
static unsigned int board_outline_id = 0;
static Shader board_borders_program = {0};
static unsigned int board_borders_id = 0;
static Shader board_fill_program = {0};
static unsigned int board_fill_id = 0;
static Shader board_unit_program = {0};
static unsigned int board_unit_id = 0;
static Shader board_unit_health_bar_program = {0};
static unsigned int board_unit_health_bar_id;
static Texture board_texture = {0};
static Texture unit_texture = {0};
static Texture health_bar_texture = {0};

static double g_mouse_pos_x = 0.0;
static double g_mouse_pos_y = 0.0;

static bool g_scroll_enabled = false;
static bool g_movement_enabled = false;

static bool g_exit_program = false;

#define SAVE_MAX_BUFFER_SIZE 2048

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
            scene_set(MAIN_MENU);
        }
        break;
    }
    case GLFW_KEY_W: {
        if (!g_movement_enabled)
        {
            break;
        }
        if (action == GLFW_PRESS)
        {
            camera_set_moving_up(true);
        }
        else if (action == GLFW_RELEASE)
        {
            camera_set_moving_up(false);
        }
        break;
    }
    case GLFW_KEY_S: {
        if (!g_movement_enabled)
        {
            break;
        }
        if (action == GLFW_PRESS)
        {
            camera_set_moving_down(true);
        }
        else if (action == GLFW_RELEASE)
        {
            camera_set_moving_down(false);
        }
        break;
    }
    case GLFW_KEY_A: {
        if (!g_movement_enabled)
        {
            break;
        }
        if (action == GLFW_PRESS)
        {
            camera_set_moving_left(true);
        }
        else if (action == GLFW_RELEASE)
        {
            camera_set_moving_left(false);
        }
        break;
    }
    case GLFW_KEY_D: {
        if (!g_movement_enabled)
        {
            break;
        }
        if (action == GLFW_PRESS)
        {
            camera_set_moving_right(true);
        }
        else if (action == GLFW_RELEASE)
        {
            camera_set_moving_right(false);
        }
        break;
    }
    case GLFW_KEY_R: {
        if (action == GLFW_PRESS)
        {
            float board_width =
                BOARD_HEX_TILE_WIDTH * current_board->board_dimension_x * 0.75 + BOARD_HEX_TILE_WIDTH / 4.0f;
            float board_height =
                BOARD_HEX_TILE_HEIGHT * current_board->board_dimension_y + BOARD_HEX_TILE_HEIGHT / 2.0f;
            vec3 position = {((float)camera_get_viewport_width() - board_width) / -2.0f,
                             ((float)camera_get_viewport_height() - board_height) / -2.0f, 2.0f};
            camera_set_position(position);
            camera_set_zoom(146.0f);
        }
        break;
    }
    case GLFW_KEY_ENTER: {
        if (action == GLFW_PRESS)
        {
            if (current_board->selected_tile_index_x >= 0 && current_board->selected_tile_index_y >= 0)
            {
                current_board
                    ->tile_ownership_status[current_board->selected_tile_index_y * current_board->board_dimension_x +
                                            current_board->selected_tile_index_x]++;
                if (current_board->tile_ownership_status[current_board->selected_tile_index_y *
                                                             current_board->board_dimension_x +
                                                         current_board->selected_tile_index_x] > 4)
                {
                    current_board->tile_ownership_status[current_board->selected_tile_index_y *
                                                             current_board->board_dimension_x +
                                                         current_board->selected_tile_index_x] = 0;
                }
                current_board->board_update_flags |= BOARD_UPDATE_BORDERS;
            }
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
    if (!g_scroll_enabled)
    {
        return;
    }
    camera_decrement_zoom((float)yoffset);
    if (camera_get_zoom() < 85.0f)
    {
        camera_set_zoom(85.0f);
    }
    else if (camera_get_zoom() > 160.0f)
    {
        camera_set_zoom(160.0f);
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

void main_game_scene_save()
{
    char *buffer = malloc(sizeof(char) * SAVE_MAX_BUFFER_SIZE);
    int buffer_index = 0;
    for (int i = 0; i < current_board->board_dimension_x * current_board->board_dimension_y; i++)
    {
        if (buffer_index >= SAVE_MAX_BUFFER_SIZE - 1)
        {
            // TODO: logging stuff
            printf("Game save failed. Buffer size too small.\n");
            free(buffer);
            return;
        }
        int size_written = sprintf(buffer + buffer_index, "%d,", current_board->tile_ownership_status[i]);
        buffer_index += size_written;
    }
    buffer[buffer_index] = ';';
    write_file("../save/save.omg", buffer, SAVE_MAX_BUFFER_SIZE);
    free(buffer);
}

void main_game_scene_load()
{
    long file_size;
    char *file_buffer = read_file("../save/save.omg", &file_size);
    char *int_buffer = malloc(sizeof(char) * 10);
    int int_buffer_index = 0;
    int tile_ownership_status_index = 0;
    for (int i = 0; i < file_size; i++)
    {
        if (file_buffer[i] == ';')
        {
            break;
        }
        if (file_buffer[i] == ',')
        {
            current_board->tile_ownership_status[tile_ownership_status_index++] = atoi(int_buffer);
            free(int_buffer);
            int_buffer = malloc(sizeof(char) * 10);
            int_buffer_index = 0;
            continue;
        }
        int_buffer[int_buffer_index++] = file_buffer[i];
    }
    free(file_buffer);
    free(int_buffer);
    board_update_border(current_board);
    basic_update_vertex_data(board_borders_id, current_board->board_border_positions, current_board->board_border_uvs,
                             current_board->board_border_colors, current_board->board_borders_count * 6);
}

void main_game_scene_reset_board()
{
    board_reset(&current_board);
    board_update_border(current_board);
    basic_update_vertex_data(board_borders_id, current_board->board_border_positions, current_board->board_border_uvs,
                             current_board->board_border_colors, current_board->board_borders_count * 6);
}

void main_game_scene_init()
{
    platform_set_callbacks(main_game_scene_glfw_framebuffer_size_callback, main_game_scene_glfw_key_callback,
                           main_game_scene_glfw_cursor_position_callback, main_game_scene_glfw_mouse_button_callback,
                           main_game_scene_glfw_scroll_callback);
    Board *board = board_create(21, 11);
    board_outline_program =
        opengl_load_basic_shaders("../resources/shaders/board_outline.vert", "../resources/shaders/board_outline.frag");
    board_outline_id = basic_vertex_data_create(
        board->board_outline_vertices, 2, NULL, NULL,
        board->board_dimension_y * (board->board_dimension_x * 2 + 2) + (board->board_dimension_x * 2 + 1),
        board->board_outline_indices, board->board_dimension_x * board->board_dimension_y * 12, 0);

    board_borders_program =
        opengl_load_basic_shaders("../resources/shaders/board_borders.vert", "../resources/shaders/board_borders.frag");
    board_borders_id = basic_vertex_data_create(board->board_border_positions, 2, board->board_border_uvs,
                                                board->board_border_colors, board->board_borders_count * 6, NULL, 0, 0);

    board_fill_program =
        opengl_load_basic_shaders("../resources/shaders/board_fill.vert", "../resources/shaders/board_fill.frag");
    board_fill_id = basic_vertex_data_create(board->board_fill_positions, 2, NULL, board->board_fill_colors,
                                             board->board_dimension_y * board->board_dimension_x * 12, NULL, 0, 0);

    board_unit_program =
        opengl_load_basic_shaders("../resources/shaders/board_borders.vert", "../resources/shaders/board_borders.frag");
    board_unit_id = basic_vertex_data_create(board->board_unit_positions, 2, board->board_unit_uvs,
                                             board->board_unit_colors, board->unit_buffer_size * 6, NULL, 0, 0);

    board_unit_health_bar_program =
        opengl_load_basic_shaders("../resources/shaders/health_bar.vert", "../resources/shaders/health_bar.frag");
    board_unit_health_bar_id =
        basic_vertex_data_create(board->board_unit_health_positions, 2, board->board_unit_health_uvs,
                                 board->board_unit_health_colors, board->unit_buffer_size * 6, NULL, 0, 0);

    current_board = board;

    board_texture = generate_opengl_texture("../resources/textures/hex/hex_borders.png");

    unit_texture = generate_opengl_texture("../resources/textures/ship/spaceships.png");

    health_bar_texture = generate_opengl_texture("../resources/textures/status_bars/white_square.png");

    camera_set_zoom(115.0f);
    float board_width = BOARD_HEX_TILE_WIDTH * current_board->board_dimension_x * 0.75 + BOARD_HEX_TILE_WIDTH / 4.0f;
    float board_height = BOARD_HEX_TILE_HEIGHT * current_board->board_dimension_y + BOARD_HEX_TILE_HEIGHT / 2.0f;

    float left = (float)camera_get_viewport_width() - (float)camera_get_viewport_width() / camera_get_zoom_ratio();
    float top = (float)camera_get_viewport_height() - (float)camera_get_viewport_height() / camera_get_zoom_ratio();
    vec3 position = {
        ((float)camera_get_viewport_width() / camera_get_zoom_ratio() - (float)camera_get_viewport_width()) -
            ((float)camera_get_viewport_width() / camera_get_zoom_ratio() - left - board_width) / 2.0f,
        (float)camera_get_viewport_height() / camera_get_zoom_ratio() - (float)camera_get_viewport_height() -
            ((float)camera_get_viewport_height() / camera_get_zoom_ratio() - top - board_height) / 2.0f,
        2.0f};
    camera_set_position(position);
    /*vec2 max_position = {((float)camera_get_viewport_width() - board_width) / -2.0f,
                         board_height / 2.0f - BOARD_HEX_TILE_HEIGHT / 2.0f};
    camera_set_max_position(max_position);
    vec2 min_position = {((float)camera_get_viewport_width() - board_width) / -2.0f, 0.0f};
    camera_set_min_position(min_position);*/
    // camera_set_zoom(155.0f);
}

void main_game_scene_update()
{
    if (g_exit_program)
    {
        main_game_scene_clean();
        platform_set_window_should_close(true);
        return;
    }
    camera_update();
    main_game_scene_update_hovered_tile();
}

void main_game_scene_render()
{
    if (g_exit_program)
    {
        return;
    }
    opengl_texture_hot_reload(&board_texture);
    opengl_shader_hot_reload(&board_outline_program);
    opengl_shader_hot_reload(&board_borders_program);
    opengl_shader_hot_reload(&board_fill_program);
    opengl_set_uniform_mat4(board_borders_program.program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(board_borders_program.program, "projection", (vec4 *)camera_get_projection());
    opengl_set_uniform_mat4(board_outline_program.program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(board_outline_program.program, "projection", (vec4 *)camera_get_projection());
    opengl_set_uniform_mat4(board_fill_program.program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(board_fill_program.program, "projection", (vec4 *)camera_get_projection());
    opengl_set_uniform_mat4(board_unit_program.program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(board_unit_program.program, "projection", (vec4 *)camera_get_projection());
    opengl_set_uniform_mat4(board_unit_health_bar_program.program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(board_unit_health_bar_program.program, "projection", (vec4 *)camera_get_projection());

    if (current_board->board_update_flags & BOARD_UPDATE_FILL)
    {
        board_update_fill_vertices(current_board);
        basic_update_vertex_data(board_fill_id, current_board->board_fill_positions, NULL,
                                 current_board->board_fill_colors,
                                 current_board->board_dimension_x * current_board->board_dimension_y * 12);
    }
    if (current_board->board_update_flags & BOARD_UPDATE_BORDERS)
    {
        board_update_border(current_board);
        basic_update_vertex_data(board_borders_id, current_board->board_border_positions,
                                 current_board->board_border_uvs, current_board->board_border_colors,
                                 current_board->board_borders_count * 6);
    }
    if (current_board->board_update_flags & BOARD_UPDATE_UNIT)
    {
        basic_update_vertex_data(board_unit_id, current_board->board_unit_positions, current_board->board_unit_uvs,
                                 current_board->board_unit_colors, current_board->unit_buffer_size * 6);
    }
    if (current_board->board_update_flags & BOARD_UPDATE_UNIT_HEALTH)
    {
        basic_update_vertex_data(board_unit_health_bar_id, current_board->board_unit_health_positions,
                                 current_board->board_unit_health_uvs, current_board->board_unit_health_colors,
                                 current_board->unit_buffer_size * 6);
    }
    glBindTexture(GL_TEXTURE_2D, board_texture.id);
    basic_draw_elements(board_outline_id, board_outline_program.program, GL_LINES);
    // basic_draw_arrays_instanced(board_borders_id, board_borders_program.program, 21 * 21);
    basic_draw_arrays(board_fill_id, board_fill_program.program, GL_TRIANGLES);
    basic_draw_arrays(board_borders_id, board_borders_program.program, GL_TRIANGLES);
    glBindTexture(GL_TEXTURE_2D, unit_texture.id);
    basic_draw_arrays(board_unit_id, board_unit_program.program, GL_TRIANGLES);
    glBindTexture(GL_TEXTURE_2D, health_bar_texture.id);
    basic_draw_arrays(board_unit_health_bar_id, board_unit_health_bar_program.program, GL_TRIANGLES);
    current_board->board_update_flags = 0;
    omega_nuklear_start();
    struct nk_context *ctx = omega_nuklear_get_nuklear_context();
    if (nk_begin(ctx, "Menu Bar", nk_rect(0.0f, 0.0f, (float)camera_get_viewport_width(), 28.0f),
                 NK_WINDOW_NO_SCROLLBAR))
    {
        nk_menubar_begin(ctx);
        nk_layout_row_begin(ctx, NK_STATIC, 25, 2);
        nk_layout_row_push(ctx, 45);
        if (nk_menu_begin_label(ctx, "File", NK_TEXT_CENTERED, nk_vec2(120, 200)))
        {
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_menu_item_label(ctx, "Save", NK_TEXT_LEFT))
            {
                main_game_scene_save();
            }
            if (nk_menu_item_label(ctx, "Load", NK_TEXT_LEFT))
            {
                main_game_scene_load();
            }
            if (nk_menu_item_label(ctx, "Reset", NK_TEXT_LEFT))
            {
                main_game_scene_reset_board();
            }
            if (nk_menu_item_label(ctx, "Exit", NK_TEXT_LEFT))
            {
                g_exit_program = true;
            }
            nk_menu_end(ctx);
        }
        nk_menubar_end(ctx);

        /*
                nk_layout_row_static(ctx, 60, 350, 1);
                char mouse_pos_str[100];
                sprintf(mouse_pos_str, "Mouse Position: %.2lf, %.2lf", g_mouse_pos_x, g_mouse_pos_y);
                nk_label(ctx, mouse_pos_str, NK_TEXT_ALIGN_LEFT);
        */
    }
    nk_end(ctx);
    if (nk_begin(ctx, "Board Information", nk_rect(50.0f, 50.0f, 200.0f, 400.0f),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
    {
        nk_layout_row_static(ctx, 30, 190, 1);
        char board_info_buffer[1000];
        sprintf(board_info_buffer, "Occupied Tiles: ");
        int buffer_index = 16;
        for (int i = 0; i < current_board->board_dimension_x * current_board->board_dimension_y; i++)
        {
            if (current_board->tile_occupation_status[i] != -1)
            {
                buffer_index += sprintf(board_info_buffer + buffer_index, "%d; ", i);
            }
        }
        nk_label(ctx, board_info_buffer, NK_TEXT_LEFT);
        char board_current_tile_buffer[50];
        sprintf(board_current_tile_buffer, "Current Tile Position: %d, %d", current_board->mouse_tile_index_x,
                current_board->mouse_tile_index_y);
        nk_label(ctx, board_current_tile_buffer, NK_TEXT_LEFT);
        char camera_position_buffer[50];
        sprintf(camera_position_buffer, "Camera Position: %.2f, %.2f", (*camera_get_position())[0],
                (*camera_get_position())[1]);
        nk_label(ctx, camera_position_buffer, NK_TEXT_LEFT);
    }
    nk_end(ctx);
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
