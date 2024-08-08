//
// Created by sterling on 7/10/24.
//

#include "platform.h"

#include "../../external/glad/glad.h"
#include "../ui/nuklear_config.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

// #define FULLSCREEN_MODE glfwGetPrimaryMonitor()
#define FULLSCREEN_MODE NULL

static GLFWwindow *g_main_window = NULL;

void platform_init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
}

int platform_create_window(int width, int height, const char *title)
{
    g_main_window = glfwCreateWindow(width, height, title, FULLSCREEN_MODE, NULL);
    if (g_main_window == NULL)
    {
        // TODO: logging stuff
        printf("Error: failed to create window");
        return -1;
    }
    glfwMakeContextCurrent(g_main_window);

    return 0;
}

void platform_set_callbacks(void *framebuffer_size, void *key, void *cursor_pos, void *mouse_button, void *scroll)
{
    glfwSetFramebufferSizeCallback(g_main_window, framebuffer_size);
    glfwSetKeyCallback(g_main_window, key);
    glfwSetCursorPosCallback(g_main_window, cursor_pos);
    glfwSetMouseButtonCallback(g_main_window, mouse_button);
    glfwSetScrollCallback(g_main_window, scroll);
}

void platform_init_nuklear()
{
    omega_nuklear_init(g_main_window);
    omega_nuklear_set_style(OMEGA_NUKLEAR_DARK);
}

bool platform_window_should_close()
{
    return glfwWindowShouldClose(g_main_window);
}

void platform_set_window_should_close(bool window_should_close)
{
    glfwSetWindowShouldClose(g_main_window, window_should_close);
}

void platform_swap_buffers()
{
    glfwSwapBuffers(g_main_window);
}

void platform_poll_events()
{
    glfwPollEvents();
}

void platform_destroy_window()
{
    glfwDestroyWindow(g_main_window);
}

void platform_clean()
{
    glfwTerminate();
}

double platform_get_time()
{
    return glfwGetTime();
}

void platform_set_vsync(int interval)
{
    glfwSwapInterval(interval);
}

int platform_get_window_width()
{
    int width, height;
    glfwGetWindowSize(g_main_window, &width, &height);
    return width;
}

int platform_get_window_height()
{
    int width, height;
    glfwGetWindowSize(g_main_window, &width, &height);
    return height;
}
