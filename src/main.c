#include "../external/glad/glad.h"
#include <GLFW/glfw3.h>
#include "scenes/scene_state.h"
#include "ui/nuklear_config.h"

#include <stdio.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define TIME_PER_UPDATE (1.0 / 60.0)

// #define FULLSCREEN_MODE glfwGetPrimaryMonitor()
#define FULLSCREEN_MODE NULL

int main(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    // glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My Game", FULLSCREEN_MODE, NULL);
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

    glEnable(GL_LINE_WIDTH);
    glLineWidth(1.0f);
    glfwSwapInterval(1);

    omega_nuklear_init(window);
    omega_nuklear_set_style(OMEGA_NUKLEAR_DARK);

    scene_set(MAIN_MENU, window);

    double previous_update_time = 0.0;
    double lag = 0.0;
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        double current_update_time = glfwGetTime();
        double elapsed_time = current_update_time - previous_update_time;
        previous_update_time = current_update_time;
        lag += elapsed_time;
        while (lag >= TIME_PER_UPDATE)
        {
            scene_update();
            lag -= TIME_PER_UPDATE;
        }
        scene_render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clean_all_vertex_data();
    omega_nuklear_shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
