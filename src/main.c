#include "../external/glad/glad.h"
#include "game.h"
#include "platform/platform.h"
#include <GLFW/glfw3.h>

#include <dlfcn.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define TIME_PER_UPDATE (1.0 / 60.0)

// #define FULLSCREEN_MODE glfwGetPrimaryMonitor()
#define FULLSCREEN_MODE NULL

void (*g_game_update_func_ptr)(void) = game_update;
void (*g_game_render_func_ptr)(void) = game_render;

void reload_game()
{
    static void *game_dl;
    static struct timespec last_edit_ts = {0};

    struct stat st;
    int ierr = stat("libOmegaApp.so", &st);
    if (ierr != 0)
    {
        printf("Error loading game so\n");
        return;
    }
    struct timespec current_ts = st.st_mtim;

    if (current_ts.tv_sec > last_edit_ts.tv_sec)
    {
        // TODO: logging stuff
        printf("Hot Reload!\n");
        sleep(10);
        if (game_dl)
        {
            dlclose(game_dl);
            game_dl = NULL;
        }
        if (copy_file("libOmegaApp.so", "libOmegaAppLoad.so"))
        {
            printf("Failed to copy file\n");
            return;
        }
        game_dl = dlopen("libOmegaAppLoad.so", RTLD_LAZY);
        g_game_update_func_ptr = dlsym(game_dl, "game_update");
        g_game_render_func_ptr = dlsym(game_dl, "game_render");
        last_edit_ts = current_ts;
        printf("Finished\n");
    }
}

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

    game_init(window);

    double previous_update_time = 0.0;
    double lag = 0.0;
    while (!glfwWindowShouldClose(window))
    {
        reload_game();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        double current_update_time = glfwGetTime();
        double elapsed_time = current_update_time - previous_update_time;
        previous_update_time = current_update_time;
        lag += elapsed_time;
        while (lag >= TIME_PER_UPDATE)
        {
            g_game_update_func_ptr();
            lag -= TIME_PER_UPDATE;
        }
        g_game_render_func_ptr();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clean_all_vertex_data();
    game_shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
