//
// Created by sterling on 7/9/24.
//

#include "main_game_scene.h"
#include "../../external/nuklear/nuklear.h"
#include "../objects/board.h"
#include "../platform/platform.h"
#include "../players/players.h"
#include "../renderer/camera.h"
#include "../renderer/opengl_renderer.h"
#include "../renderer/opengl_shader.h"
#include "../renderer/opengl_texture.h"
#include "../ui/nuklear_config.h"
#include "../util/file_helpers.h"
#include "../util/general_helpers.h"
#include "scene_state.h"

#include <stdbool.h>
#include <string.h>
#include <time.h>

static Board *g_current_board = NULL;
static Players *g_players = NULL;

static Shader g_board_outline_program = {0};
static unsigned int g_board_outline_id = 0;
static Shader g_board_borders_program = {0};
static unsigned int g_board_borders_id = 0;
static Shader g_board_fill_program = {0};
static unsigned int g_board_fill_id = 0;
static Shader g_board_unit_program = {0};
static unsigned int g_board_unit_id = 0;
static Shader g_board_unit_health_bar_program = {0};
static unsigned int g_board_unit_health_bar_id;
static Shader g_board_planet_program = {0};
static unsigned int g_board_planet_id = 0;
static Texture g_board_texture = {0};
static Texture g_unit_texture = {0};
static Texture g_health_bar_texture = {0};
static Texture g_planet_texture = {0};

static double g_mouse_pos_x = 0.0;
static double g_mouse_pos_y = 0.0;

static bool g_scroll_enabled = true;
static bool g_movement_enabled = true;

static bool g_exit_program = false;

#define SAVE_MAX_BUFFER_SIZE 4096

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
        if (g_current_board->mouse_tile_index_x == -1 || g_current_board->mouse_tile_index_y == -1)
        {
            break;
        }
        if (action == GLFW_PRESS)
        {
            int unit_index =
                g_current_board->units->unit_tile_occupation_status[g_current_board->mouse_tile_index_y *
                                                                        g_current_board->board_dimension_x +
                                                                    g_current_board->mouse_tile_index_x];
            if (unit_index != -1)
            {
                int *new_owner = &g_current_board->units->unit_owner[unit_index];
                (*new_owner)++;
                if (*new_owner > 4)
                {
                    *new_owner = 1;
                }
                unit_update_color(g_current_board->units, unit_index);
            }
        }
        break;
    }
    case GLFW_KEY_E: {
        if (g_current_board->mouse_tile_index_x == -1 || g_current_board->mouse_tile_index_y == -1)
        {
            break;
        }
        if (action == GLFW_PRESS)
        {
            int unit_index =
                g_current_board->units->unit_tile_occupation_status[g_current_board->mouse_tile_index_y *
                                                                        g_current_board->board_dimension_x +
                                                                    g_current_board->mouse_tile_index_x];
            if (unit_index == -1)
            {
                unit_add(g_current_board->units, g_current_board->board_current_turn % 4 + 1, 0,
                         g_current_board->mouse_tile_index_x, g_current_board->mouse_tile_index_y,
                         g_current_board->board_dimension_x, g_current_board->board_dimension_y);
            }
            else
            {
                unit_remove(g_current_board->units, unit_index, g_current_board->mouse_tile_index_x,
                            g_current_board->mouse_tile_index_y, g_current_board->board_dimension_x);
            }
        }
        break;
    }
    case GLFW_KEY_T: {
        if (g_current_board->mouse_tile_index_x == -1 || g_current_board->mouse_tile_index_y == -1)
        {
            break;
        }
        if (action == GLFW_PRESS)
        {
            int unit_index =
                g_current_board->units->unit_tile_occupation_status[g_current_board->mouse_tile_index_y *
                                                                        g_current_board->board_dimension_x +
                                                                    g_current_board->mouse_tile_index_x];
            if (unit_index != -1)
            {
                g_current_board->units->unit_type[unit_index]++;
                if (g_current_board->units->unit_type[unit_index] >= TOTAL_UNIT_TYPES)
                {
                    g_current_board->units->unit_type[unit_index] = 0;
                }
                unit_update_uv(g_current_board->units, unit_index);
            }
        }
        break;
    }
    case GLFW_KEY_Q: {
        if (action == GLFW_PRESS)
        {
            if (g_current_board->selected_tile_index_x >= 0 && g_current_board->selected_tile_index_y >= 0)
            {
                g_current_board->units->unit_tile_ownership_status[g_current_board->selected_tile_index_y *
                                                                       g_current_board->board_dimension_x +
                                                                   g_current_board->selected_tile_index_x]++;
                if (g_current_board->units->unit_tile_ownership_status[g_current_board->selected_tile_index_y *
                                                                           g_current_board->board_dimension_x +
                                                                       g_current_board->selected_tile_index_x] > 4)
                {
                    g_current_board->units->unit_tile_ownership_status[g_current_board->selected_tile_index_y *
                                                                           g_current_board->board_dimension_x +
                                                                       g_current_board->selected_tile_index_x] = 0;
                }
                g_current_board->board_update_flags |= BOARD_UPDATE_BORDERS;
            }
        }
        break;
    }
    case GLFW_KEY_F: {
        if (g_current_board->mouse_tile_index_x == -1 || g_current_board->mouse_tile_index_y == -1 ||
            g_current_board->selected_tile_index_x == -1 || g_current_board->selected_tile_index_y == -1)
        {
            break;
        }
        if (action == GLFW_PRESS)
        {
            if (g_current_board->board_highlighted_path != NULL)
            {
                da_int_free(g_current_board->board_highlighted_path);
                free(g_current_board->board_highlighted_path);
            }
            g_current_board->board_highlighted_path = hex_grid_find_path(
                g_current_board, g_current_board->selected_tile_index_x, g_current_board->selected_tile_index_y,
                g_current_board->mouse_tile_index_x, g_current_board->mouse_tile_index_y,
                g_current_board->board_dimension_x, g_current_board->board_dimension_y);
            board_update_fill_vertices(g_current_board);
        }
        break;
    }
    case GLFW_KEY_ENTER: {
        if (action == GLFW_PRESS)
        {
            player_end_turn(g_current_board, g_players, g_current_board->board_current_turn % 4);
            player_start_turn(g_current_board, g_players, g_current_board->board_current_turn % 4);
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
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT: {
        if (action == GLFW_PRESS)
        {
            board_handle_tile_click(g_current_board);
        }
        break;
    case GLFW_MOUSE_BUTTON_MIDDLE: {
        if (action == GLFW_PRESS)
        {
            camera_set_zoom(155.0f);
            float board_width =
                BOARD_HEX_TILE_WIDTH * g_current_board->board_dimension_x * 0.75 + BOARD_HEX_TILE_WIDTH / 4.0f;
            float board_height =
                BOARD_HEX_TILE_HEIGHT * g_current_board->board_dimension_y + BOARD_HEX_TILE_HEIGHT / 2.0f;

            float left =
                (float)camera_get_viewport_width() - (float)camera_get_viewport_width() / camera_get_zoom_ratio();
            float top =
                (float)camera_get_viewport_height() - (float)camera_get_viewport_height() / camera_get_zoom_ratio();
            vec3 position = {
                ((float)camera_get_viewport_width() / camera_get_zoom_ratio() - (float)camera_get_viewport_width()) -
                    ((float)camera_get_viewport_width() / camera_get_zoom_ratio() - left - board_width) / 2.0f,
                (float)camera_get_viewport_height() / camera_get_zoom_ratio() - (float)camera_get_viewport_height() -
                    ((float)camera_get_viewport_height() / camera_get_zoom_ratio() - top - board_height) / 2.0f,
                2.0f};
            camera_set_position(position);

            break;
        }
    }
    case GLFW_MOUSE_BUTTON_RIGHT: {
        break;
    }
    default:
        break;
    }
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
    else if (camera_get_zoom() > 210.0f)
    {
        camera_set_zoom(210.0f);
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
    board_update_hovered_tile(g_current_board, modified_x_pos + (*camera_get_position())[0] + left,
                              modified_y_pos + (*camera_get_position())[1] + top);
}

bool main_game_scene_write_int_array(char *buffer, int *buffer_index, int *int_arr, int count)
{
    for (int i = 0; i < count; i++)
    {
        char test_str[12];
        int size_written = sprintf(test_str, "%d,", int_arr[i]);
        if (*buffer_index >= SAVE_MAX_BUFFER_SIZE - size_written - 2)
        {
            // TODO: logging stuff
            printf("Game save failed. Buffer size too small.\n");
            return false;
        }
        if (size_written == 0)
        {
            printf("Game save failed. Failed to write to test_str.\n");
            return false;
        }
        strcpy(buffer + *buffer_index, test_str);
        *buffer_index += size_written;
    }
    buffer[(*buffer_index)++] = ';';
    buffer[(*buffer_index)++] = '\n';
    return true;
}

bool main_game_scene_write_float_array(char *buffer, int *buffer_index, float *float_arr, int count)
{
    for (int i = 0; i < count; i++)
    {
        char test_str[12];
        int size_written = sprintf(test_str, "%.4f,", float_arr[i]);
        if (*buffer_index >= SAVE_MAX_BUFFER_SIZE - size_written - 2)
        {
            // TODO: logging stuff
            printf("Game save failed. Buffer size too small.\n");
            return false;
        }
        if (size_written == 0)
        {
            printf("Game save failed. Failed to write to test_str.\n");
            return false;
        }
        strcpy(buffer + *buffer_index, test_str);
        *buffer_index += size_written;
    }
    buffer[(*buffer_index)++] = ';';
    buffer[(*buffer_index)++] = '\n';
    return true;
}

void main_game_scene_save()
{
    char *buffer = malloc(sizeof(char) * SAVE_MAX_BUFFER_SIZE);
    if (buffer == NULL)
    {
        printf("Game save failed. Failed to allocate buffer.\n");
        return;
    }
    int buffer_index = 0;
    if (!main_game_scene_write_int_array(buffer, &buffer_index, &g_current_board->board_dimension_x, 1))
    {
        free(buffer);
        return;
    }
    if (!main_game_scene_write_int_array(buffer, &buffer_index, &g_current_board->board_dimension_y, 1))
    {
        free(buffer);
        return;
    }
    if (!main_game_scene_write_int_array(buffer, &buffer_index, g_current_board->units->unit_tile_ownership_status,
                                         g_current_board->board_dimension_x * g_current_board->board_dimension_y))
    {
        free(buffer);
        return;
    }
    if (!main_game_scene_write_int_array(buffer, &buffer_index, &g_current_board->board_current_turn, 1))
    {
        free(buffer);
        return;
    }
    if (!main_game_scene_write_int_array(buffer, &buffer_index, &g_current_board->units->unit_buffer_size, 1))
    {
        free(buffer);
        return;
    }
    if (!main_game_scene_write_int_array(buffer, &buffer_index, g_current_board->units->unit_owner,
                                         g_current_board->units->unit_buffer_size))
    {
        free(buffer);
        return;
    }
    if (!main_game_scene_write_float_array(buffer, &buffer_index, g_current_board->units->unit_health,
                                           g_current_board->units->unit_buffer_size))
    {
        free(buffer);
        return;
    }
    if (!main_game_scene_write_int_array(buffer, &buffer_index, g_current_board->units->unit_tile_occupation_status,
                                         g_current_board->board_dimension_x * g_current_board->board_dimension_y))
    {
        free(buffer);
        return;
    }
    if (!main_game_scene_write_int_array(buffer, &buffer_index, g_current_board->units->unit_freed_indices.array,
                                         (int)g_current_board->units->unit_freed_indices.used))
    {
        free(buffer);
        return;
    }
    if (!main_game_scene_write_float_array(buffer, &buffer_index, g_current_board->units->unit_movement_points,
                                           g_current_board->units->unit_buffer_size))
    {
        free(buffer);
        return;
    }

    write_file("../save/save.omg", buffer, SAVE_MAX_BUFFER_SIZE);
    free(buffer);
}

bool main_game_scene_read_int_array(const char *file_buffer, int *file_index, int **int_arr, int new_size)
{
    free(*int_arr);
    *int_arr = malloc(sizeof(int) * new_size);
    char *int_buffer = malloc(sizeof(char) * 12);
    memset(int_buffer, 0, sizeof(char) * 12);
    int int_buffer_index = 0;
    int int_arr_index = 0;
    while (true)
    {
        if (*file_index >= SAVE_MAX_BUFFER_SIZE)
        {
            printf("Game load error. Index out of bounds.\n");
            return false;
        }
        if (file_buffer[*file_index] == ';')
        {
            (*file_index) += 2;
            break;
        }
        if (file_buffer[*file_index] == ',')
        {
            (*int_arr)[int_arr_index] = (int)strtol(int_buffer, NULL, 10);
            free(int_buffer);
            int_buffer = malloc(sizeof(char) * 12);
            memset(int_buffer, 0, sizeof(char) * 12);
            int_buffer_index = 0;
            int_arr_index++;
            (*file_index)++;
            continue;
        }
        int_buffer[int_buffer_index++] = file_buffer[*file_index];
        (*file_index)++;
    }
    free(int_buffer);
    return true;
}

bool main_game_scene_read_float_array(const char *file_buffer, int *file_index, float **float_arr, int new_size)
{
    free(*float_arr);
    *float_arr = malloc(sizeof(float) * new_size);
    char *float_buffer = malloc(sizeof(char) * 12);
    memset(float_buffer, 0, sizeof(char) * 12);
    int float_buffer_index = 0;
    int float_arr_index = 0;
    while (true)
    {
        if (*file_index >= SAVE_MAX_BUFFER_SIZE)
        {
            printf("Game load error. Index out of bounds.\n");
            return false;
        }
        if (file_buffer[*file_index] == ';')
        {
            (*file_index) += 2;
            break;
        }
        if (file_buffer[*file_index] == ',')
        {
            (*float_arr)[float_arr_index] = strtof(float_buffer, NULL);
            free(float_buffer);
            float_buffer = malloc(sizeof(char) * 12);
            memset(float_buffer, 0, sizeof(char) * 12);
            float_buffer_index = 0;
            float_arr_index++;
            (*file_index)++;
            continue;
        }
        float_buffer[float_buffer_index++] = file_buffer[*file_index];
        (*file_index)++;
    }
    free(float_buffer);
    return true;
}

bool main_game_scene_read_dynamic_int_array(const char *file_buffer, int *file_index, DynamicIntArray *int_arr)
{
    char *int_buffer = malloc(sizeof(char) * 12);
    memset(int_buffer, 0, sizeof(char) * 12);
    int int_buffer_index = 0;
    int int_arr_index = 0;
    while (true)
    {
        if (*file_index >= SAVE_MAX_BUFFER_SIZE)
        {
            printf("Game load error. Index out of bounds.\n");
            return false;
        }
        if (file_buffer[*file_index] == ';')
        {
            (*file_index) += 2;
            break;
        }
        if (file_buffer[*file_index] == ',')
        {
            int tmp_int = (int)strtol(int_buffer, NULL, 10);
            da_int_push_back(int_arr, tmp_int);
            free(int_buffer);
            int_buffer = malloc(sizeof(char) * 12);
            memset(int_buffer, 0, sizeof(char) * 12);
            int_buffer_index = 0;
            int_arr_index++;
            (*file_index)++;
            continue;
        }
        int_buffer[int_buffer_index++] = file_buffer[*file_index];
        (*file_index)++;
    }
    free(int_buffer);
    return true;
}

void main_game_scene_load()
{
    long file_size;
    char *file_buffer = read_file("../save/save.omg", &file_size);
    int file_index = 0;
    int *saved_board_dimension_x;
    int *saved_board_dimension_y;
    if (!main_game_scene_read_int_array(file_buffer, &file_index, &saved_board_dimension_x, 1))
    {
        free(file_buffer);
        return;
    }
    if (!main_game_scene_read_int_array(file_buffer, &file_index, &saved_board_dimension_y, 1))
    {
        free(file_buffer);
        return;
    }
    if (g_current_board->board_dimension_x != *saved_board_dimension_x ||
        g_current_board->board_dimension_y != *saved_board_dimension_y)
    {
        board_reset_new_dimensions(&g_current_board, *saved_board_dimension_x, *saved_board_dimension_y);
    }
    if (!main_game_scene_read_int_array(file_buffer, &file_index, &g_current_board->units->unit_tile_ownership_status,
                                        g_current_board->board_dimension_x * g_current_board->board_dimension_y))
    {
        free(file_buffer);
        return;
    }
    int *tmp_int = malloc(sizeof(int));
    if (!main_game_scene_read_int_array(file_buffer, &file_index, &tmp_int, 1))
    {
        free(file_buffer);
        return;
    }
    g_current_board->board_current_turn = *tmp_int;
    if (!main_game_scene_read_int_array(file_buffer, &file_index, &tmp_int, 1))
    {
        free(file_buffer);
        return;
    }
    g_current_board->units->unit_buffer_size = *tmp_int;
    free(tmp_int);
    if (!main_game_scene_read_int_array(file_buffer, &file_index, &g_current_board->units->unit_owner,
                                        g_current_board->units->unit_buffer_size))
    {
        free(file_buffer);
        return;
    }
    if (!main_game_scene_read_float_array(file_buffer, &file_index, &g_current_board->units->unit_health,
                                          g_current_board->units->unit_buffer_size))
    {
        free(file_buffer);
        return;
    }
    if (!main_game_scene_read_int_array(file_buffer, &file_index, &g_current_board->units->unit_tile_occupation_status,
                                        g_current_board->board_dimension_x * g_current_board->board_dimension_y))
    {
        free(file_buffer);
        return;
    }
    if (!main_game_scene_read_dynamic_int_array(file_buffer, &file_index, &g_current_board->units->unit_freed_indices))
    {
        free(file_buffer);
        return;
    }
    if (!main_game_scene_read_float_array(file_buffer, &file_index, &g_current_board->units->unit_movement_points,
                                          g_current_board->units->unit_buffer_size))
    {
        free(file_buffer);
        return;
    }

    free(file_buffer);
    board_update_border(g_current_board);
    unit_realloc(g_current_board->units);
    unit_clear_vertices(g_current_board->units);
    for (int i = 0; i < g_current_board->board_dimension_x * g_current_board->board_dimension_y; i++)
    {
        int unit_index = g_current_board->units->unit_tile_occupation_status[i];
        if (unit_index != -1)
        {
            int x = i % g_current_board->board_dimension_x;
            int y = i / g_current_board->board_dimension_x;
            unit_update_position(g_current_board->units, unit_index, x, y);
            unit_update_uv(g_current_board->units, unit_index);
            unit_update_color(g_current_board->units, unit_index);
            unit_update_health_position(g_current_board->units, unit_index);
            unit_update_health_uv(g_current_board->units, unit_index);
            unit_update_health_color(g_current_board->units, unit_index);
        }
    }
}

void main_game_scene_reset_board()
{
    board_reset(&g_current_board);
    board_update_border(g_current_board);
    player_start_turn(g_current_board, g_players, g_current_board->board_current_turn % 4);
}

void main_game_scene_init()
{
    platform_set_callbacks(main_game_scene_glfw_framebuffer_size_callback, main_game_scene_glfw_key_callback,
                           main_game_scene_glfw_cursor_position_callback, main_game_scene_glfw_mouse_button_callback,
                           main_game_scene_glfw_scroll_callback);
    g_players = players_create(MAX_PLAYERS, 1, NULL);
    Board *board = board_create(21, 21, MAX_PLAYERS);
    g_board_outline_program =
        opengl_load_basic_shaders("../resources/shaders/board_outline.vert", "../resources/shaders/board_outline.frag");
    g_board_outline_id = basic_vertex_data_create(
        board->board_outline_vertices, 2, NULL, NULL,
        board->board_dimension_y * (board->board_dimension_x * 2 + 2) + (board->board_dimension_x * 2 + 1),
        board->board_outline_indices, board->board_dimension_x * board->board_dimension_y * 12, 0);

    g_board_borders_program =
        opengl_load_basic_shaders("../resources/shaders/board_borders.vert", "../resources/shaders/board_borders.frag");
    g_board_borders_id =
        basic_vertex_data_create(board->board_border_positions, 2, board->board_border_uvs, board->board_border_colors,
                                 board->board_borders_count * 6, NULL, 0, 0);

    g_board_fill_program =
        opengl_load_basic_shaders("../resources/shaders/board_fill.vert", "../resources/shaders/board_fill.frag");
    g_board_fill_id = basic_vertex_data_create(board->board_fill_positions, 2, NULL, board->board_fill_colors,
                                               board->board_dimension_y * board->board_dimension_x * 12, NULL, 0, 0);

    g_board_unit_program =
        opengl_load_basic_shaders("../resources/shaders/board_borders.vert", "../resources/shaders/board_borders.frag");
    g_board_unit_id =
        basic_vertex_data_create(board->units->unit_positions, 2, board->units->unit_uvs, board->units->unit_colors,
                                 board->units->unit_buffer_size * 6, NULL, 0, 0);

    g_board_unit_health_bar_program =
        opengl_load_basic_shaders("../resources/shaders/health_bar.vert", "../resources/shaders/health_bar.frag");
    g_board_unit_health_bar_id =
        basic_vertex_data_create(board->units->unit_health_positions, 2, board->units->unit_health_uvs,
                                 board->units->unit_health_colors, board->units->unit_buffer_size * 6, NULL, 0, 0);

    g_board_planet_program =
        opengl_load_basic_shaders("../resources/shaders/board_borders.vert", "../resources/shaders/board_borders.frag");
    g_board_planet_id =
        basic_vertex_data_create(board->planets->planet_positions, 2, board->planets->planet_uvs,
                                 board->planets->planet_colors, board->planets->planet_buffer_size * 6, NULL, 0, 0);

    g_current_board = board;

    g_board_texture = generate_opengl_texture("../resources/textures/hex/hex_borders.png");

    g_unit_texture = generate_opengl_texture("../resources/textures/ship/spaceships_v3.png");

    g_health_bar_texture = generate_opengl_texture("../resources/textures/status_bars/white_square.png");

    g_planet_texture = generate_opengl_texture("../resources/textures/planets/planets_v2.png");

    camera_set_zoom(155.0f);
    float board_width = BOARD_HEX_TILE_WIDTH * g_current_board->board_dimension_x * 0.75 + BOARD_HEX_TILE_WIDTH / 4.0f;
    float board_height = BOARD_HEX_TILE_HEIGHT * g_current_board->board_dimension_y + BOARD_HEX_TILE_HEIGHT / 2.0f;

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

    // TODO: look into getting better random
    srand(time(NULL));

    player_start_turn(g_current_board, g_players, g_current_board->board_current_turn % 4);
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
    opengl_texture_hot_reload(&g_board_texture);
    opengl_shader_hot_reload(&g_board_outline_program);
    opengl_shader_hot_reload(&g_board_borders_program);
    opengl_shader_hot_reload(&g_board_fill_program);
    opengl_set_uniform_mat4(g_board_borders_program.program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(g_board_borders_program.program, "projection", (vec4 *)camera_get_projection());
    opengl_set_uniform_mat4(g_board_outline_program.program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(g_board_outline_program.program, "projection", (vec4 *)camera_get_projection());
    opengl_set_uniform_mat4(g_board_fill_program.program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(g_board_fill_program.program, "projection", (vec4 *)camera_get_projection());
    opengl_set_uniform_mat4(g_board_unit_program.program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(g_board_unit_program.program, "projection", (vec4 *)camera_get_projection());
    opengl_set_uniform_mat4(g_board_unit_health_bar_program.program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(g_board_unit_health_bar_program.program, "projection", (vec4 *)camera_get_projection());
    opengl_set_uniform_mat4(g_board_planet_program.program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(g_board_planet_program.program, "projection", (vec4 *)camera_get_projection());

    if (g_current_board->board_update_flags & BOARD_UPDATE_FILL)
    {
        board_update_fill_vertices(g_current_board);
        basic_update_vertex_data(g_board_fill_id, g_current_board->board_fill_positions, NULL,
                                 g_current_board->board_fill_colors,
                                 g_current_board->board_dimension_x * g_current_board->board_dimension_y * 12);
    }
    if (g_current_board->board_update_flags & BOARD_UPDATE_BORDERS)
    {
        board_update_border(g_current_board);
        basic_update_vertex_data(g_board_borders_id, g_current_board->board_border_positions,
                                 g_current_board->board_border_uvs, g_current_board->board_border_colors,
                                 g_current_board->board_borders_count * 6);
    }
    if (g_current_board->units->unit_update_flags & UNIT_UPDATE)
    {
        basic_update_vertex_data(g_board_unit_id, g_current_board->units->unit_positions,
                                 g_current_board->units->unit_uvs, g_current_board->units->unit_colors,
                                 g_current_board->units->unit_buffer_size * 6);
    }
    if (g_current_board->units->unit_update_flags & UNIT_UPDATE_HEALTH)
    {
        basic_update_vertex_data(g_board_unit_health_bar_id, g_current_board->units->unit_health_positions,
                                 g_current_board->units->unit_health_uvs, g_current_board->units->unit_health_colors,
                                 g_current_board->units->unit_buffer_size * 6);
    }
    if (g_current_board->planets->planet_update_flags & PLANET_UPDATE)
    {
        basic_update_vertex_data(g_board_planet_id, g_current_board->planets->planet_positions,
                                 g_current_board->planets->planet_uvs, g_current_board->planets->planet_colors,
                                 g_current_board->planets->planet_buffer_size * 6);
    }
    glBindTexture(GL_TEXTURE_2D, g_board_texture.id);
    basic_draw_elements(g_board_outline_id, g_board_outline_program.program, GL_LINES);
    basic_draw_arrays(g_board_fill_id, g_board_fill_program.program, GL_TRIANGLES);
    basic_draw_arrays(g_board_borders_id, g_board_borders_program.program, GL_TRIANGLES);
    glBindTexture(GL_TEXTURE_2D, g_planet_texture.id);
    basic_draw_arrays(g_board_planet_id, g_board_planet_program.program, GL_TRIANGLES);
    glBindTexture(GL_TEXTURE_2D, g_unit_texture.id);
    basic_draw_arrays(g_board_unit_id, g_board_unit_program.program, GL_TRIANGLES);
    glBindTexture(GL_TEXTURE_2D, g_health_bar_texture.id);
    basic_draw_arrays(g_board_unit_health_bar_id, g_board_unit_health_bar_program.program, GL_TRIANGLES);
    g_current_board->board_update_flags = 0;
    g_current_board->units->unit_update_flags = 0;
    g_current_board->planets->planet_update_flags = 0;
    omega_nuklear_start();
    struct nk_context *ctx = omega_nuklear_get_nuklear_context();
    if (nk_begin(ctx, "Menu Bar", nk_rect(0.0f, 0.0f, (float)camera_get_viewport_width(), 28.0f),
                 NK_WINDOW_NO_SCROLLBAR))
    {
        nk_menubar_begin(ctx);
        nk_layout_row_begin(ctx, NK_STATIC, 25, 3);
        nk_layout_row_push(ctx, 80);
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
        if (nk_menu_begin_label(ctx, "Turn", NK_TEXT_CENTERED, nk_vec2(120, 200)))
        {
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_menu_item_label(ctx, "Next Turn", NK_TEXT_LEFT))
            {
                player_end_turn(g_current_board, g_players, g_current_board->board_current_turn % 4);
                player_start_turn(g_current_board, g_players, g_current_board->board_current_turn % 4);
            }
            nk_menu_end(ctx);
        }
        char turn_count_buffer[30];
        sprintf(turn_count_buffer, "Turn %d",
                g_current_board->player_count != 0 ? g_current_board->board_current_turn / g_current_board->player_count
                                                   : 0);
        nk_label(ctx, turn_count_buffer, NK_TEXT_CENTERED);
        nk_menubar_end(ctx);

        /*
                nk_layout_row_static(ctx, 60, 350, 1);
                char mouse_pos_str[100];
                sprintf(mouse_pos_str, "Mouse Position: %.2lf, %.2lf", g_mouse_pos_x, g_mouse_pos_y);
                nk_label(ctx, mouse_pos_str, NK_TEXT_ALIGN_LEFT);
        */
    }
    nk_end(ctx);
    if (nk_begin(ctx, "Board Information", nk_rect(50.0f, 50.0f, 250.0f, 400.0f),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
    {
        nk_layout_row_static(ctx, 30, 250, 1);
        char board_current_tile_buffer[50];
        sprintf(board_current_tile_buffer, "Current Tile Position: %d, %d", g_current_board->mouse_tile_index_x,
                g_current_board->mouse_tile_index_y);
        nk_label(ctx, board_current_tile_buffer, NK_TEXT_LEFT);
        char camera_position_buffer[50];
        sprintf(camera_position_buffer, "Camera Position: %.2f, %.2f", (*camera_get_position())[0],
                (*camera_get_position())[1]);
        nk_label(ctx, camera_position_buffer, NK_TEXT_LEFT);
    }
    nk_end(ctx);
    if (nk_begin(ctx, "Player Information", nk_rect(1280.0f - 300.0f, 50.0f, 250.0f, 400.0f),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
    {

        int player_index;
        if (g_players->human_players_count == 1)
        {
            player_index = g_players->human_players[0];
        }
        else
        {
            player_index = g_current_board->board_current_turn % 4;
        }
        if (is_human_player(g_players, player_index))
        {
            nk_layout_row_static(ctx, 30, 250, 1);
            int radioactive_materials = g_players->resources->radioactive_material_count[player_index];
            int rare_metals = g_players->resources->rare_metals_count[player_index];
            int common_metals = g_players->resources->common_metals_count[player_index];
            char player_info_buffer[40];
            sprintf(player_info_buffer, "Radioactive Materials: %d", radioactive_materials);
            nk_label(ctx, player_info_buffer, NK_TEXT_LEFT);
            sprintf(player_info_buffer, "Rare Metals: %d", rare_metals);
            nk_label(ctx, player_info_buffer, NK_TEXT_LEFT);
            sprintf(player_info_buffer, "Common Metals: %d", common_metals);
            nk_label(ctx, player_info_buffer, NK_TEXT_LEFT);
            sprintf(player_info_buffer, "Score: %d", g_players->player_score[player_index]);
            nk_label(ctx, player_info_buffer, NK_TEXT_LEFT);
        }
    }
    nk_end(ctx);
    omega_nuklear_end();
}

void main_game_scene_clean()
{
    clean_all_vertex_data();
    glDeleteProgram(g_board_borders_program.program);
    glDeleteProgram(g_board_outline_program.program);
    glDeleteProgram(g_board_fill_program.program);
    board_destroy(g_current_board);
    g_current_board = NULL;
}
