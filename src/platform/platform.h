//
// Created by sterling on 7/10/24.
//

#ifndef OMEGAAPP_PLATFORM_H
#define OMEGAAPP_PLATFORM_H
#include <stdbool.h>

void platform_init();

int platform_create_window(int width, int height, const char *title);

int platform_create_share_window(const char *title);

void switch_to_main_context();

void switch_to_loading_context();

void platform_set_callbacks(void *framebuffer_size, void *key, void *cursor_pos, void *mouse_button, void *scroll);

void platform_init_nuklear();

bool platform_window_should_close();

void platform_set_window_should_close(bool window_should_close);

void platform_swap_buffers();

void platform_swap_buffers_loading();

void platform_destroy_window();

void platform_destroy_loading_window();

void platform_clean();

void platform_poll_events();

double platform_get_time();

void platform_set_vsync(int interval);

#endif // OMEGAAPP_PLATFORM_H
