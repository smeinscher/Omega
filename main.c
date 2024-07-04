#include "external/glad/glad.h"
#include <GLFW/glfw3.h>
#include "game/board.h"
#include "renderer/opengl_renderer.h"
#include "renderer/opengl_shader.h"

#include <cglm/cglm.h>

#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

static float g_zoom_factor = 100.0f;
static float g_zoom = 100.0f;

int main(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My Game", NULL, NULL);
    if (window == NULL)
    {
        // TODO: logging stuff
        printf("Error: failed to create window");
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        // TODO: logging stuff
        printf("Error: failed to initiate glad");
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSwapInterval(1);

    unsigned int board_program = basic_shader_create("resources/shaders/board_outline.vert",
                                                     "resources/shaders/board_outline.frag");
    float *board_vertices = board_vertices_create(21, 21, 100, 0, 25.0f, 25.0f);
    unsigned int *board_indices = board_outline_indices_create(21, 21);
    int board_id = basic_vertex_data_create(board_vertices, 2, NULL, NULL, 21 * (21 * 2 + 2) * 2, board_indices,
                                            21 * 21 * 12, 0);

    float left = (float)WINDOW_WIDTH - (float)WINDOW_WIDTH / (g_zoom_factor / g_zoom);
    float right = (float)WINDOW_WIDTH / (g_zoom_factor / g_zoom);
    float bottom = (float)WINDOW_HEIGHT / (g_zoom_factor / g_zoom);
    float top = (float)WINDOW_HEIGHT - (float)WINDOW_HEIGHT / (g_zoom_factor / g_zoom);
    mat4 projection;
    glm_ortho(left, right, bottom, top, 0.1f, 10.0f, projection);
    mat4 view;
    vec3 camera_position = {0.0f, 0.0f, 2.0f};
    vec3 forward = {0.0f, 0.0f, -1.0f};
    vec3 up = {0.0f, 1.0f, 0.0f};
    vec3 camera_position_plus_forward;
    glm_vec3_add(camera_position, forward, camera_position_plus_forward);
    glm_lookat(camera_position, camera_position_plus_forward, up, view);
    opengl_set_uniform_mat4(board_program, "projection", projection);
    opengl_set_uniform_mat4(board_program, "view", view);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        basic_draw_elements(board_id, board_program, GL_LINES);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}
