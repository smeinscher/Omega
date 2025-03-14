//
// Created by sterling on 5/31/24.
//

#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H

#include <bits/types/struct_timespec.h>
#include <cglm/cglm.h>

typedef struct Shader
{
    unsigned int program;
    const char *vertex_shader_path;
    const char *fragment_shader_path;

    struct timespec vertex_shader_last_modified_timestamp;
    struct timespec fragment_shader_last_modified_timestamp;
} Shader;

Shader opengl_load_basic_shaders(const char *vertex_shader_path, const char *fragment_shader_path);

void opengl_set_uniform_float(unsigned int program, const char *name, float f);

void opengl_set_uniform_4f(unsigned int program, const char *name, float x, float y, float z, float w);

void opengl_set_uniform_mat4(unsigned int program, const char *name, mat4 mat);

void opengl_shader_hot_reload(Shader *shader);

#endif // OPENGL_SHADER_H
