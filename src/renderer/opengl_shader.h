//
// Created by sterling on 5/31/24.
//

#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H

#include <cglm/cglm.h>

unsigned int opengl_load_basic_shaders(const char *vertex_shader_path, const char *fragment_shader_path);

void opengl_set_uniform_mat4(unsigned int program, const char *name, mat4 mat);

#endif //OPENGL_SHADER_H
