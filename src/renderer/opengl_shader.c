//
// Created by sterling on 5/31/24.
//

#include "opengl_shader.h"

#include <stdio.h>

#include "../../external/glad/glad.h"
#include "../util/file_helpers.h"

#include <stdlib.h>

#define MAX_LOG_BUFFER_SIZE 512

unsigned int opengl_compile_shader(const char *path, unsigned int type);

void opengl_attach_shaders(unsigned int shader_program, unsigned int vertex_shader, unsigned int fragment_shader)
{
}

Shader opengl_load_basic_shaders(const char *vertex_shader_path, const char *fragment_shader_path)
{
    Shader shader = {0};
    const unsigned int vertex_shader = opengl_compile_shader(vertex_shader_path, GL_VERTEX_SHADER);
    if (vertex_shader == 0)
    {
        // TODO: logging stuff
        printf("Vertex shader failed to compile\n");
        return shader;
    }
    const unsigned int fragment_shader = opengl_compile_shader(fragment_shader_path, GL_FRAGMENT_SHADER);
    if (fragment_shader == 0)
    {
        // TODO: logging stuff
        printf("Fragment shader failed to compile");
        return shader;
    }

    unsigned int shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    int success;
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char info_log[MAX_LOG_BUFFER_SIZE];
        glGetProgramInfoLog(shader_program, MAX_LOG_BUFFER_SIZE, NULL, info_log);
        // TODO: logging stuff
        printf("Failed to link shader_program:\n\t%s", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glUseProgram(shader_program);

    shader.program = shader_program;
    shader.vertex_shader_path = vertex_shader_path;
    shader.fragment_shader_path = fragment_shader_path;
    shader.vertex_shader_last_modified_timestamp = get_file_timestamp(vertex_shader_path);
    shader.fragment_shader_last_modified_timestamp = get_file_timestamp(fragment_shader_path);
    return shader;
}

unsigned int opengl_compile_shader(const char *path, unsigned int type)
{
    char *source = NULL;
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
    {
        // TODO: logging stuff
        printf("Failed to open file\n");
        return 0;
    }

    if (fseek(fp, 0L, SEEK_END) == 0)
    {
        const long buffer_size = ftell(fp);
        if (buffer_size == -1)
        {
            // TODO: logging stuff
            printf("Failed to get file size\n");
            return 0;
        }
        source = malloc(sizeof(char) * (buffer_size + 1));
        if (fseek(fp, 0L, SEEK_SET) != 0)
        {
            // TODO: logging stuff
            printf("Failed to return to start of file\n");
            return 0;
        }

        size_t new_length = fread(source, sizeof(char), buffer_size, fp);
        if (ferror(fp) != 0)
        {
            // TODO: logging stuff
            printf("Error reading file\n");
            return 0;
        }
        source[new_length++] = '\0';
    }
    fclose(fp);
    const unsigned int shader = glCreateShader(type);

    const char *shader_source = source;
    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);

    free(source);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char info_log[MAX_LOG_BUFFER_SIZE];
        glGetShaderInfoLog(shader, MAX_LOG_BUFFER_SIZE, NULL, info_log);
        printf("Failed to compile shader\n\t%s", info_log);
        return 0;
    }
    return shader;
}

void opengl_set_uniform_float(unsigned int program, const char *name, float f)
{
    glUseProgram(program);
    glUniform1f(glGetUniformLocation(program, name), f);
}

void opengl_set_uniform_mat4(unsigned int program, const char *name, mat4 mat)
{
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, &mat[0][0]);
}

void opengl_shader_hot_reload(Shader *shader)
{
    struct timespec vertex_current_timestamp = get_file_timestamp(shader->vertex_shader_path);
    struct timespec fragment_current_timestamp = get_file_timestamp(shader->fragment_shader_path);

    if (vertex_current_timestamp.tv_sec > shader->vertex_shader_last_modified_timestamp.tv_sec ||
        fragment_current_timestamp.tv_sec > shader->fragment_shader_last_modified_timestamp.tv_sec)
    {
        // TODO: logging stuff
        printf("Hot reloading shader!");
        glDeleteProgram(shader->program);
        *shader = opengl_load_basic_shaders(shader->vertex_shader_path, shader->fragment_shader_path);
    }
}
