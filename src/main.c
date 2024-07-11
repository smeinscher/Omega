#include "game/game.h"
#include "util/file_helpers.h"

#include <dlfcn.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720


void (*g_game_update_func_ptr)(void) = game_update;

void (*g_game_render_func_ptr)(void) = game_render;

void (*g_game_shutdown_func_ptr)(void) = game_shutdown;

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
        sleep(2);
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
        g_game_shutdown_func_ptr = dlsym(game_dl, "game_shutdown");
        last_edit_ts = current_ts;
        // void (*game_init_func_ptr)(GLFWwindow *) = dlsym(game_dl, "game_init");
        // game_init_func_ptr(g_main_window);
        printf("Finished\n");
    }
}

int main(int argc, char *argv[])
{
    if (game_init(WINDOW_WIDTH, WINDOW_HEIGHT, "Omega"))
    {
        printf("Error initializing game\n");
        return -1;
    }

    while (game_is_running())
    {
        reload_game();
        g_game_update_func_ptr();
        g_game_render_func_ptr();
    }

    g_game_shutdown_func_ptr();
    return 0;
}
