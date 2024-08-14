//
// Created by sterling on 8/13/24.
//

#ifndef OMEGAAPP_OPENGL_FREETYPE_WRAPPER_H
#define OMEGAAPP_OPENGL_FREETYPE_WRAPPER_H

#include "opengl_shader.h"
#include <stdbool.h>

bool opengl_freetype_wrapper_load_font(const char *font_path);

Shader *opengl_freetype_wrapper_get_font_shader();

void opengl_freetype_wrapper_render_text(const char *text, int text_length, float x, float y, float scale, float r,
                                         float g, float b, float a);

#endif // OMEGAAPP_OPENGL_FREETYPE_WRAPPER_H
