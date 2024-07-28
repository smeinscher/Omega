//
// Created by sterling on 7/11/24.
//

#include "loader.h"
#include "../../external/glad/glad.h"
#include "../platform/platform.h"
#include "../renderer/opengl_renderer.h"
#include "../renderer/opengl_shader.h"
#include "../renderer/opengl_texture.h"
#include <stddef.h>

Texture g_texture;
Shader g_shader;

void loading_screen(const bool *done_loading)
{
    g_texture = generate_opengl_texture("../resources/textures/loading_background.png");

    g_shader =
        opengl_load_basic_shaders("../resources/shaders/board_borders.vert", "../resources/shaders/board_borders.frag");

    float bg_img_positions[12];
    bg_img_positions[0] = 0.0f;
    bg_img_positions[1] = 720.0f;
    bg_img_positions[2] = 1280.0f;
    bg_img_positions[3] = 0.0f;
    bg_img_positions[4] = 0.0f;
    bg_img_positions[5] = 0.0f;
    bg_img_positions[6] = 0.0f;
    bg_img_positions[7] = 720.0f;
    bg_img_positions[8] = 1280.0f;
    bg_img_positions[9] = 0.0f;
    bg_img_positions[10] = 1280.0f;
    bg_img_positions[11] = 720.0f;

    float bg_img_uvs[12];
    bg_img_uvs[0] = 0.0f;
    bg_img_uvs[1] = 1.0f;
    bg_img_uvs[2] = 1.0f;
    bg_img_uvs[3] = 0.0f;
    bg_img_uvs[4] = 0.0f;
    bg_img_uvs[5] = 0.0f;
    bg_img_uvs[6] = 0.0f;
    bg_img_uvs[7] = 1.0f;
    bg_img_uvs[8] = 1.0f;
    bg_img_uvs[9] = 0.0f;
    bg_img_uvs[10] = 1.0f;
    bg_img_uvs[11] = 1.0f;

    float bg_img_colors[24];
    for (int i = 0; i < 6; i++)
    {
        bg_img_colors[i * 4] = 0.7f;
        bg_img_colors[i * 4 + 1] = 0.7f;
        bg_img_colors[i * 4 + 2] = 0.7f;
        bg_img_colors[i * 4 + 3] = 1.0f;
    }

    unsigned int bg_img_id = basic_vertex_data_create(bg_img_positions, 2, bg_img_uvs, bg_img_colors, 6, NULL, 0, 0);

    glBindTexture(GL_TEXTURE_2D, g_texture.id);
    vec3 camera_position = {0.0f, 0.0f, 2.0f};
    vec3 forward = {0.0f, 0.0f, -1.0f};
    vec3 up = {0.0f, 1.0f, 0.0f};
    vec3 camera_position_plus_forward;
    glm_vec3_add(camera_position, forward, camera_position_plus_forward);
    mat4 view;
    glm_lookat(camera_position, camera_position_plus_forward, up, view);
    mat4 projection;
    glm_ortho(0, 1280, 720, 0, 0.1f, 10.0f, projection);
    opengl_set_uniform_mat4(g_shader.program, "view", view);
    opengl_set_uniform_mat4(g_shader.program, "projection", projection);
    while (!(*done_loading))
    {
        platform_poll_events();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        basic_draw_arrays(bg_img_id, g_shader.program, GL_TRIANGLES);
        platform_swap_buffers();
    }

    clean_vertex_data(bg_img_id);
    glDeleteProgram(g_shader.program);
}
