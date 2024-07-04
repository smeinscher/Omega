//
// Created by sterling on 6/25/24.
//

#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include <stddef.h>

#define OMEGA_GAME_MAX_VERTEX_DATA_COUNT 8

typedef struct BasicVertexData
{
    unsigned int vao;
    unsigned int vbos[3];
    unsigned int ebo;
    unsigned int count;
    unsigned int dimensions;
    unsigned int indices_count;
    float *positions;
    float *uvs;
    float *colors;
    unsigned int *indices;
} BasicVertexData;

unsigned int basic_vertex_data_create(float *positions, unsigned int dimensions, float *uvs, float *colors,
                                      unsigned int vertex_count, unsigned int *indices, unsigned int indices_count,
                                      unsigned int dynamic_draw_flag);

unsigned int basic_shader_create(const char *vertex_shader_path, const char *fragment_shader_path);

void basic_draw_arrays(unsigned int vertex_data_id, unsigned int program, unsigned int mode);

void basic_draw_elements(unsigned int vertex_data_id, unsigned int program, unsigned int mode);

void clean_vertex_data(unsigned int vertex_data_id);

void clean_all_vertex_data();

#endif //OPENGL_RENDERER_H
