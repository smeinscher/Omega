//
// Created by sterling on 7/9/24.
//

#include "main_game_scene.h"
#include "../../external/nuklear/nuklear.h"
#include "../objects/board.h"
#include "../objects/star_background.h"
#include "../objects/text.h"
#include "../platform/platform.h"
#include "../players/ai_actions.h"
#include "../players/players.h"
#include "../renderer/camera.h"
#include "../renderer/opengl_freetype_wrapper.h"
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
static StarBackground *g_star_background = NULL;

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
static Shader g_star_background_program = {0};
static unsigned int g_star_background_id = 0;

static Texture g_board_texture = {0};
static Texture g_unit_texture = {0};
static Texture g_health_bar_texture = {0};
static Texture g_planet_texture = {0};

static double g_mouse_pos_x = 0.0;
static double g_mouse_pos_y = 0.0;

static bool g_scroll_enabled = true;
static bool g_movement_enabled = true;

static bool g_exit_program = false;

static bool g_pause = false;

static int g_win_count[4] = {0};

static int g_place_unit_type = -1;

static int g_stash_x = -1;
static int g_stash_y = -1;

static TextData *g_text_data;

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
            board_update_highlighted_path_fill_vertices(g_current_board);
        }
        break;
    }
    case GLFW_KEY_P: {
        if (action == GLFW_PRESS)
        {
            g_pause = !g_pause;
        }
        break;
    }
    case GLFW_KEY_ENTER: {
        if (action == GLFW_PRESS)
        {
            if (g_current_board->units->moves_unit_index.used == 0)
            {
                if (is_human_player(g_players, g_current_board->board_current_turn % 4))
                {
                    player_end_turn(g_current_board, g_players, g_current_board->board_current_turn % 4);
                }
                board_process_planet_orbit(g_current_board);
                player_start_turn(g_current_board, g_players, g_current_board->board_current_turn % 4);
            }
        }
        break;
    }
    case GLFW_KEY_SPACE: {
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
                int player_index = g_current_board->board_current_turn % 4;
                unit_purchase(player_index, g_players->resources, g_current_board->units, DROID,
                              g_current_board->mouse_tile_index_x, g_current_board->mouse_tile_index_y,
                              g_current_board->board_dimension_x, g_current_board->board_dimension_y);
            }
        }
        break;
    }
    case GLFW_KEY_LEFT_ALT: {
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
                int player_index = g_current_board->board_current_turn % 4;
                unit_purchase(player_index, g_players->resources, g_current_board->units, BATTLESHIP,
                              g_current_board->mouse_tile_index_x, g_current_board->mouse_tile_index_y,
                              g_current_board->board_dimension_x, g_current_board->board_dimension_y);
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
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT: {
        if (action == GLFW_PRESS)
        {
            if (g_place_unit_type != -1)
            {
                board_handle_tile_placement(g_current_board, g_place_unit_type, g_players->player_count,
                                            &g_players->player_score[g_current_board->board_current_turn % 4]);
                g_place_unit_type = -1;
            }
            else
            {
                board_handle_tile_click(g_current_board);
            }
        }
        break;
    case GLFW_MOUSE_BUTTON_MIDDLE: {
        if (!g_scroll_enabled)
        {
            break;
        }
        if (action == GLFW_PRESS)
        {
            camera_set_zoom(155.0f);
            float board_width = board_get_hex_tile_width() * g_current_board->board_dimension_x * 0.75 +
                                board_get_hex_tile_width() / 4.0f;
            float board_height =
                board_get_hex_tile_height() * g_current_board->board_dimension_y + board_get_hex_tile_height() / 2.0f;

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
    g_current_board->board_update_flags |= BOARD_UPDATE_BORDERS;
    players_reset(&g_players);
    g_pause = g_players->human_players_count == 0;
    if (g_players->human_players_count == 0)
    {
        player_start_turn(g_current_board, g_players, 0);
    }
}

void main_game_scene_init()
{
    // TODO: look into getting better random
    srand(time(NULL));

    camera_set_viewport_width(platform_get_window_width());
    camera_set_viewport_height(platform_get_window_height());
    platform_set_callbacks(main_game_scene_glfw_framebuffer_size_callback, main_game_scene_glfw_key_callback,
                           main_game_scene_glfw_cursor_position_callback, main_game_scene_glfw_mouse_button_callback,
                           main_game_scene_glfw_scroll_callback);
    g_star_background = star_background_create();
    g_players = players_create(MAX_PLAYERS, 1, NULL);
    g_pause = g_players->human_players_count == 0;
    Board *board = board_create(19, 19, MAX_PLAYERS);
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
        opengl_load_basic_shaders("../resources/shaders/planets.vert", "../resources/shaders/planets.frag");
    g_board_planet_id =
        basic_vertex_data_create(board->planets->planet_positions, 2, board->planets->planet_uvs,
                                 board->planets->planet_colors, board->planets->planet_buffer_size * 6, NULL, 0, 0);

    g_star_background_program = opengl_load_basic_shaders("../resources/shaders/star_background.vert",
                                                          "../resources/shaders/star_background.frag");
    g_star_background_id = basic_vertex_data_create(g_star_background->star_background_positions, 2, NULL,
                                                    g_star_background->star_background_colors, 6, NULL, 0, 0);

    g_current_board = board;

    g_board_texture = generate_opengl_texture("../resources/textures/hex/hex_borders.png");

    g_unit_texture = generate_opengl_texture("../resources/textures/ship/spaceships_v3.png");

    g_health_bar_texture = generate_opengl_texture("../resources/textures/status_bars/white_square.png");

    g_planet_texture = generate_opengl_texture("../resources/textures/planets/planets_v2.png");

    camera_set_zoom(155.0f);
    float board_width = (float)board_get_hex_tile_width() * (float)g_current_board->board_dimension_x * 0.75f +
                        (float)board_get_hex_tile_width() / 4.0f;
    float board_height = (float)board_get_hex_tile_height() * (float)g_current_board->board_dimension_y +
                         (float)board_get_hex_tile_height() / 2.0f;

    float left = (float)camera_get_viewport_width() - (float)camera_get_viewport_width() / camera_get_zoom_ratio();
    float top = (float)camera_get_viewport_height() - (float)camera_get_viewport_height() / camera_get_zoom_ratio();
    vec3 position = {
        ((float)camera_get_viewport_width() / camera_get_zoom_ratio() - (float)camera_get_viewport_width()) -
            ((float)camera_get_viewport_width() / camera_get_zoom_ratio() - left - board_width) / 2.0f,
        (float)camera_get_viewport_height() / camera_get_zoom_ratio() - (float)camera_get_viewport_height() -
            ((float)camera_get_viewport_height() / camera_get_zoom_ratio() - top - board_height) / 2.0f,
        2.0f};
    camera_set_position(position);

    opengl_freetype_wrapper_load_font("../resources/fonts/HomeVideo/HomeVideo-BLG6G.ttf");
    g_text_data = text_data_create();
    units_set_text_data(g_text_data);
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

    /*
        for (int i = 0; i < g_current_board->units->display_info_unit_index.used; i++)
        {
            char *text = string_to_c_str(&g_current_board->units->unit_display_info.array[i]);
            if (text[0] == '-')
            {
                continue;
            }
            int unit_index = g_current_board->units->display_info_unit_index.array[i];
            float x = g_current_board->units->unit_positions[unit_index * 12] - BOARD_HEX_TILE_WIDTH / 2.0f;
            float y = g_current_board->units->unit_positions[unit_index * 12 + 3];
            text_data_add(g_text_data, text, g_current_board->units->unit_display_info.array[i].used, x, y, 0.4f, 0.0f,
                          1.0f, 0.0f, 1.0f, g_current_board->units->unit_display_info_time.array[i]);
            free(text);
            da_int_remove(&g_current_board->units->display_info_unit_index, i);
            da_string_remove(&g_current_board->units->unit_display_info, i);
            da_float_remove(&g_current_board->units->unit_display_info_time, i);
            i--;
        }
    */
    text_data_update(g_text_data);

    if (g_current_board->units->moves_unit_index.used != 0)
    {
        int unit_index = g_current_board->units->moves_unit_index.array[0];
        int start_x = g_current_board->units->moves_list.array[0];
        int start_y = g_current_board->units->moves_list.array[1];
        int end_x = g_current_board->units->moves_list.array[2];
        int end_y = g_current_board->units->moves_list.array[3];
        int move_type = g_current_board->units->move_type.array[0];

        if (is_human_player(g_players, g_current_board->board_current_turn % 4) &&
            g_current_board->units->unit_animation_progress < 0.0f)
        {
            g_current_board->units->unit_animation_progress = 0.0f;
        }
        bool animation_complete = false;
        if (move_type != NOTHING)
        {
            animation_complete = unit_animate_movement(g_current_board->units);
        }
        else
        {
            animation_complete = true;
            da_int_remove(&g_current_board->units->moves_unit_index, 0);
            da_int_remove(&g_current_board->units->moves_list, 0);
            da_int_remove(&g_current_board->units->moves_list, 0);
            da_int_remove(&g_current_board->units->moves_list, 0);
            da_int_remove(&g_current_board->units->moves_list, 0);
            da_int_remove(&g_current_board->units->move_type, 0);
            g_current_board->units->unit_animation_progress = -1.0f;
        }
        if (animation_complete)
        {
            switch (move_type)
            {
            case INVADE: {
                //                units_process_display_text(g_current_board->units, g_text_data, end_x, end_y,
                //                                           g_current_board->board_dimension_x);
                /*bool destroy_self;
                for (int i = 0; i < g_current_board->units->unit_remove_list.used; i++)
                {
                    int remove_unit_index = g_current_board->units->unit_remove_list.array[i];
                    destroy_self = remove_unit_index == unit_index;
                    if (end_x == g_current_board->units->unit_indices[remove_unit_index * 2] &&
                        end_y == g_current_board->units->unit_indices[remove_unit_index * 2 + 1])
                    {
                        unit_remove(g_current_board->units, remove_unit_index, end_x, end_y,
                                    g_current_board->board_dimension_x);
                        da_int_remove(&g_current_board->units->unit_remove_list, i);
                        break;
                    }
                }*/
                unit_move(g_current_board->units, unit_index, NULL, end_x, end_y, g_current_board->board_dimension_x,
                          g_current_board->board_dimension_y);
                g_current_board->board_update_flags |= BOARD_UPDATE_BORDERS;
                unit_occupy_new_tile(g_current_board->units, unit_index, start_x, start_y, end_x, end_y,
                                     g_current_board->board_dimension_x);
                /*
                                else
                                {
                                    unit_remove(g_current_board->units, unit_index, start_x, start_y,
                                                g_current_board->board_dimension_x);
                                }
                */
                break;
            }
            case REGULAR: {
                DynamicIntArray *move_path =
                    hex_grid_find_path(g_current_board, start_x, start_y, end_x, end_y,
                                       g_current_board->board_dimension_x, g_current_board->board_dimension_y);
                if (move_path == NULL)
                {
                    printf("oof\n");
                }
                unit_move(g_current_board->units, unit_index, move_path, end_x, end_y,
                          g_current_board->board_dimension_x, g_current_board->board_dimension_y);
                unit_occupy_new_tile(g_current_board->units, unit_index, start_x, start_y, end_x, end_y,
                                     g_current_board->board_dimension_x);
                g_current_board->board_update_flags |= BOARD_UPDATE_BORDERS;
                da_int_free(move_path);
                free(move_path);
                break;
            }
            case ATTACK: {
                g_stash_x = start_x;
                g_stash_y = start_y;
                switch (board_process_attack(
                    g_current_board,
                    g_current_board->units
                        ->unit_tile_occupation_status[end_y * g_current_board->board_dimension_x + end_x],
                    unit_index))
                {
                case NO_UNITS_DESTROYED: {
                    DynamicIntArray *da =
                        hex_grid_find_path(g_current_board, start_x, start_y, end_x, end_y,
                                           g_current_board->board_dimension_x, g_current_board->board_dimension_y);
                    if (da == NULL)
                    {
                        printf("ruh row\n");
                        break;
                    }
                    if (da->used >= 2)
                    {
                        unit_add_movement_animation(g_current_board->units, unit_index, end_x, end_y, da->array[0],
                                                    da->array[1], RETREAT);
                        unit_occupy_new_tile(g_current_board->units, unit_index, start_x, start_y, da->array[0],
                                             da->array[1], g_current_board->board_dimension_x);
                    }
                    else
                    {
                        unit_add_movement_animation(g_current_board->units, unit_index, end_x, end_y, start_x, start_y,
                                                    RETREAT);
                    }
                    da_int_free(da);
                    free(da);
                    break;
                }
                case DEFENDER_DESTROYED:
                    unit_move(g_current_board->units, unit_index, NULL, end_x, end_y,
                              g_current_board->board_dimension_x, g_current_board->board_dimension_y);
                    g_current_board->board_update_flags |= BOARD_UPDATE_BORDERS;
                    unit_occupy_new_tile(g_current_board->units, unit_index, start_x, start_y, end_x, end_y,
                                         g_current_board->board_dimension_x);
                    break;
                default:
                    break;
                }
                //                units_process_display_text(g_current_board->units, g_text_data, end_x, end_y,
                //                                           g_current_board->board_dimension_x);
                break;
            }
            case RETREAT: {
                DynamicIntArray *move_path =
                    hex_grid_find_path(g_current_board, g_stash_x, g_stash_y, end_x, end_y,
                                       g_current_board->board_dimension_x, g_current_board->board_dimension_y);
                unit_move(g_current_board->units, unit_index, move_path, end_x, end_y,
                          g_current_board->board_dimension_x, g_current_board->board_dimension_y);
                g_current_board->board_update_flags |= BOARD_UPDATE_BORDERS;
                da_int_free(move_path);
                free(move_path);
                break;
            }
            default:
                break;
            }
        }
    }
    else // if (!is_human_player(g_players, g_current_board->board_current_turn % 4))
    {
        if (!g_pause)
        {
            if (!is_human_player(g_players, g_current_board->board_current_turn % 4) &&
                !(g_players->player_flags & PLAYER_ENDED_TURN))
            {
                if (ai_process_next_move(g_current_board, g_players, g_current_board->board_current_turn % 4))
                {
                    player_end_turn(g_current_board, g_players, g_current_board->board_current_turn % 4);
                }
            }
            else if (g_players->player_flags & PLAYER_ENDED_TURN)
            {
                board_process_planet_orbit(g_current_board);
                units_process_actions(g_current_board->units, g_current_board->board_current_turn / 4,
                                      g_current_board->board_dimension_x, g_current_board->board_dimension_y);
                player_start_turn(g_current_board, g_players, g_current_board->board_current_turn % 4);
            }
        }
    }
    int total_eliminated_players = 0;
    int possible_winner = -1;
    for (int i = 0; i < g_players->player_count; i++)
    {
        if (g_players->player_unit_count[i] == 0)
        {
            total_eliminated_players++;
        }
        else
        {
            possible_winner = i;
        }
    }
    if (total_eliminated_players >= 3)
    {
        if (possible_winner > -1)
        {
            g_win_count[possible_winner]++;
        }
        main_game_scene_reset_board();
        g_pause = false;
    }
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
    opengl_shader_hot_reload(&g_star_background_program);

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
    opengl_set_uniform_mat4(opengl_freetype_wrapper_get_font_shader()->program, "view", (vec4 *)camera_get_view());
    opengl_set_uniform_mat4(opengl_freetype_wrapper_get_font_shader()->program, "projection",
                            (vec4 *)camera_get_projection());
    static float time = 0.0f;
    time += 0.05f;
    opengl_set_uniform_float(g_star_background_program.program, "time", time);

    if (g_current_board->board_update_flags & BOARD_UPDATE_FILL)
    {
        board_clear_fill_vertices(g_current_board);
        board_update_hovered_tile_fill_vertices(g_current_board);
        board_update_selected_tile_fill_vertices(g_current_board);
        board_update_highlighted_path_fill_vertices(g_current_board);
        if (is_human_player(g_players, g_current_board->board_current_turn % 4))
        {
            board_update_moveable_unit_fill_vertices(g_current_board);
            board_update_moveable_tile_fill_vertices(g_current_board);
            board_update_attackable_tile_fill_vertices(g_current_board);
            board_update_swapable_tile_fill_vertices(g_current_board);
        }
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
    basic_draw_arrays(g_star_background_id, g_star_background_program.program, GL_TRIANGLES);
    glBindTexture(GL_TEXTURE_2D, g_board_texture.id);
    basic_draw_elements(g_board_outline_id, g_board_outline_program.program, GL_LINES);
    basic_draw_arrays(g_board_fill_id, g_board_fill_program.program, GL_TRIANGLES);
    basic_draw_arrays(g_board_borders_id, g_board_borders_program.program, GL_TRIANGLES);

    // glBindTexture(GL_TEXTURE_2D, g_planet_texture.id);
    basic_draw_arrays(g_board_planet_id, g_board_planet_program.program, GL_TRIANGLES);
    glBindTexture(GL_TEXTURE_2D, g_unit_texture.id);
    basic_draw_arrays(g_board_unit_id, g_board_unit_program.program, GL_TRIANGLES);
    glBindTexture(GL_TEXTURE_2D, g_health_bar_texture.id);
    basic_draw_arrays(g_board_unit_health_bar_id, g_board_unit_health_bar_program.program, GL_TRIANGLES);
    g_current_board->board_update_flags = 0;
    g_current_board->units->unit_update_flags = 0;
    g_current_board->planets->planet_update_flags = 0;
    text_data_render(g_text_data);
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
                if (g_current_board->units->moves_unit_index.used == 0)
                {
                    if (is_human_player(g_players, g_current_board->board_current_turn % 4))
                    {
                        player_end_turn(g_current_board, g_players, g_current_board->board_current_turn % 4);
                    }
                    board_process_planet_orbit(g_current_board);
                    player_start_turn(g_current_board, g_players, g_current_board->board_current_turn % 4);
                }
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
    float margin_edge = 25.0f;
    float margin_board = 25.0f;
    float margin_top = 50.0f;
    float margin_y = 25.0f;
    float width = (float)platform_get_window_width() / 4.0f - margin_edge - margin_board;
    float height = (float)platform_get_window_height() / 2.0f - margin_y * 2.0f;
    if (nk_begin(ctx, "Tile Information", nk_rect(margin_edge, margin_top, width, height),
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_static(ctx, 30, 250, 1);
        char board_current_tile_buffer[50];
        // TODO: for player-facing stuff, add one to tile indices
        sprintf(board_current_tile_buffer, "Tile: %d, %d", g_current_board->mouse_tile_index_x,
                g_current_board->mouse_tile_index_y);
        nk_label(ctx, board_current_tile_buffer, NK_TEXT_LEFT);
        int q, r;
        hex_grid_offset_to_axial(g_current_board->mouse_tile_index_x - g_current_board->board_dimension_x / 2,
                                 g_current_board->mouse_tile_index_y - g_current_board->board_dimension_y / 2, 0, &q,
                                 &r);
        sprintf(board_current_tile_buffer, "Tile q,r: %d, %d", q, r);
        nk_label(ctx, board_current_tile_buffer, NK_TEXT_LEFT);
    }
    nk_end(ctx);
    if (nk_begin(ctx, "Tile Actions",
                 nk_rect(margin_edge, (float)platform_get_window_height() / 2.0f + margin_y, width, height),
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_static(ctx, 25, 250, 1);

        if ((g_current_board->selected_tile_index_x != -1 && g_current_board->selected_tile_index_y != -1) ||
            (g_current_board->last_selected_tile_index_x != -1 && g_current_board->last_selected_tile_index_y != -1))
        {
            int selected_unit;
            if (g_current_board->selected_tile_index_x != -1 && g_current_board->selected_tile_index_y != -1)
            {
                selected_unit =
                    g_current_board->units->unit_tile_occupation_status[g_current_board->selected_tile_index_y *
                                                                            g_current_board->board_dimension_x +
                                                                        g_current_board->selected_tile_index_x];
            }
            else
            {
                selected_unit =
                    g_current_board->units->unit_tile_occupation_status[g_current_board->last_selected_tile_index_y *
                                                                            g_current_board->board_dimension_x +
                                                                        g_current_board->last_selected_tile_index_x];
            }
            if (selected_unit != -1 && g_current_board->units->unit_type[selected_unit] == WORKER &&
                g_current_board->units->unit_owner[selected_unit] ==
                    g_current_board->board_current_turn % g_players->player_count + 1)
            {
                if (unit_doing_action(g_current_board->units, selected_unit, UNIT_BUILD_STATION))
                {
                    int turn = unit_get_current_action_starting_turn(g_current_board->units, selected_unit);
                    char building_station_turn_buffer[40];
                    // TODO: make 5 a variable somewhere
                    sprintf(building_station_turn_buffer, "Building Station (%d turns left)",
                            5 - (g_current_board->board_current_turn / g_current_board->player_count - turn));
                    nk_label(ctx, building_station_turn_buffer, NK_TEXT_ALIGN_LEFT);
                }
                else if (nk_button_label(ctx, "Create Station"))
                {
                    board_worker_build_station(g_current_board, selected_unit);
                }
            }
            if (selected_unit != -1 && g_current_board->units->unit_type[selected_unit] == STATION &&
                g_current_board->units->unit_owner[selected_unit] ==
                    g_current_board->board_current_turn % g_players->player_count + 1 &&
                nk_button_label(ctx, "Create Worker"))
            {
                board_highlight_possible_unit_placement(g_current_board, selected_unit,
                                                        g_current_board->last_selected_tile_index_x,
                                                        g_current_board->last_selected_tile_index_y);
                g_place_unit_type = WORKER;
            }
            if (selected_unit != -1 && g_current_board->units->unit_type[selected_unit] == STATION &&
                g_current_board->units->unit_owner[selected_unit] ==
                    g_current_board->board_current_turn % g_players->player_count + 1 &&
                nk_button_label(ctx, "Create Droid"))
            {
                board_highlight_possible_unit_placement(g_current_board, selected_unit,
                                                        g_current_board->last_selected_tile_index_x,
                                                        g_current_board->last_selected_tile_index_y);
                g_place_unit_type = DROID;
            }
            if (selected_unit != -1 && g_current_board->units->unit_type[selected_unit] == STATION &&
                g_current_board->units->unit_owner[selected_unit] ==
                    g_current_board->board_current_turn % g_players->player_count + 1 &&
                nk_button_label(ctx, "Create Battleship"))
            {
                board_highlight_possible_unit_placement(g_current_board, selected_unit,
                                                        g_current_board->last_selected_tile_index_x,
                                                        g_current_board->last_selected_tile_index_y);
                g_place_unit_type = BATTLESHIP;
            }
        }
    }
    nk_end(ctx);
    if (nk_begin(ctx, "Player Information",
                 nk_rect((float)platform_get_window_width() - width - margin_edge, margin_top, width, height),
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_static(ctx, 8, 250, 1);
        for (int i = 0; i < g_players->player_count; i++)
        {
            char player_info_buffer[40];
            sprintf(player_info_buffer, "Player %d", i);
            nk_label(ctx, player_info_buffer, NK_TEXT_LEFT);
            sprintf(player_info_buffer, "Score: %d", g_players->player_score[i]);
            nk_label(ctx, player_info_buffer, NK_TEXT_LEFT);
            sprintf(player_info_buffer, "Unit Count: %d", g_players->player_unit_count[i]);
            nk_label(ctx, player_info_buffer, NK_TEXT_LEFT);
        }
    }
    nk_end(ctx);
    if (nk_begin(ctx, "General Information",
                 nk_rect((float)platform_get_window_width() - width - margin_edge,
                         (float)platform_get_window_height() / 2.0f + margin_y, width, height),
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_static(ctx, 8, 250, 1);
        char buffer[30];
        sprintf(buffer, "Turn %d",
                g_current_board->player_count != 0 ? g_current_board->board_current_turn / g_current_board->player_count
                                                   : 0);
        nk_label(ctx, buffer, NK_TEXT_LEFT);
        for (int i = 0; i < g_players->player_count; i++)
        {
            sprintf(buffer, "Player %d Win Count: %d", i, g_win_count[i]);
            nk_label(ctx, buffer, NK_TEXT_LEFT);
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
    players_destroy(g_players);
    g_players = NULL;
    text_data_destroy(g_text_data);
    g_text_data = NULL;
}
