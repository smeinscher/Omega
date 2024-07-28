#include "game/game.h"
#include "game/loader.h"
#include "util/file_helpers.h"

#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

void (*g_game_update_func_ptr)(void) = game_update;

void (*g_game_render_func_ptr)(void) = game_render;

void (*g_game_shutdown_func_ptr)(void) = game_shutdown;

static void *game_dl = NULL;
static struct timespec last_edit_ts = {0};

void *hot_reload_thread(void *vargp)
{
    bool *done_loading = (bool *)vargp;
    printf("Hot Reload!\n");
    sleep(3);
    if (game_dl)
    {
        dlclose(game_dl);
        game_dl = NULL;
    }
    if (copy_file("libOmegaApp.so", "libOmegaAppLoad.so"))
    {
        printf("Failed to copy file\n");
        return NULL;
    }
    game_dl = dlopen("libOmegaAppLoad.so", RTLD_LAZY);
    *done_loading = true;
    printf("Finished\n");
    return NULL;
}

void reload_game()
{
    struct timespec current_ts = get_file_timestamp("libOmegaApp.so");
    if (current_ts.tv_sec > last_edit_ts.tv_sec)
    {
        bool done_loading = false;
        pthread_t loading_thread_id;
        pthread_create(&loading_thread_id, NULL, hot_reload_thread, &done_loading);
        loading_screen(&done_loading);
        pthread_join(loading_thread_id, NULL);

        void (*game_reload_func_ptr)(void *) = dlsym(game_dl, "game_reload");
        game_reload_func_ptr(game_dl);
        g_game_update_func_ptr = dlsym(game_dl, "game_update");
        g_game_render_func_ptr = dlsym(game_dl, "game_render");
        g_game_shutdown_func_ptr = dlsym(game_dl, "game_shutdown");
        last_edit_ts = current_ts;
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
