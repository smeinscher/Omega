//
// Created by sterling on 7/10/24.
//

#ifndef NUKLEAR_CONFIG_H
#define NUKLEAR_CONFIG_H

#include "../../external/glad/glad.h"
#include <GLFW/glfw3.h>

#define MAX_VERTEX_BUFFER (512 * 1024)
#define MAX_ELEMENT_BUFFER (128 * 1024)

typedef enum OmegaStyle
{
    OMEGA_NUKLEAR_DARK
} OmegaStyle;

void omega_nuklear_init(GLFWwindow *main_window);

void omega_nuklear_set_style(OmegaStyle style);

struct nk_context *omega_nuklear_get_nuklear_context();

void omega_nuklear_start();

void omega_nuklear_end();

void omega_nuklear_shutdown();

#endif //NUKLEAR_CONFIG_H
