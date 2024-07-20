//
// Created by sterling on 6/25/24.
//

#include "opengl_renderer.h"

#include "../../external/glad/glad.h"
#include "opengl_shader.h"

#include <GLFW/glfw3.h>
#include <stdlib.h>

static BasicVertexData g_basic_vertex_data[OMEGA_GAME_MAX_VERTEX_DATA_COUNT];
static unsigned int g_basic_vertex_data_size = 0;

int opengl_glad_init()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        // TODO: logging stuff
        printf("Error: failed to initiate glad");
        return -1;
    }
    return 0;
}

void opengl_enable_default_attributes()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_WIDTH);
    glLineWidth(1.0f);
}

unsigned int basic_vertex_data_create(float *positions, unsigned int dimensions, float *uvs, float *colors,
                                      unsigned int vertex_count, float *instance_offsets,
                                      unsigned int instance_offsets_count, unsigned int *indices,
                                      unsigned int indices_count, unsigned int dynamic_draw_flag)
{
    if (g_basic_vertex_data_size >= OMEGA_GAME_MAX_VERTEX_DATA_COUNT)
    {
        // TODO: logging stuff
        printf("Reached max number of vertex data (%d)", OMEGA_GAME_MAX_VERTEX_DATA_COUNT);
        return 0;
    }
    BasicVertexData vertex_data;
    vertex_data.positions = positions;
    vertex_data.dimensions = dimensions;
    vertex_data.uvs = uvs;
    vertex_data.colors = colors;
    vertex_data.count = vertex_count;
    vertex_data.instance_offsets = instance_offsets;
    vertex_data.instance_offsets_count = instance_offsets_count;
    vertex_data.indices = indices;
    vertex_data.indices_count = indices_count;
    glGenVertexArrays(1, &vertex_data.vao);
    glGenBuffers(1, &vertex_data.vbos[0]);
    if (uvs != NULL)
    {
        glGenBuffers(1, &vertex_data.vbos[1]);
    }
    if (colors != NULL)
    {
        glGenBuffers(1, &vertex_data.vbos[2]);
    }
    if (instance_offsets != NULL)
    {
        glGenBuffers(1, &vertex_data.vbos[3]);
    }
    if (indices_count > 0)
    {
        glGenBuffers(1, &vertex_data.ebo);
    }
    else
    {
        vertex_data.ebo = 0;
    }

    unsigned int gl_usage;
    if (dynamic_draw_flag == 0)
    {
        gl_usage = GL_STATIC_DRAW;
    }
    else
    {
        gl_usage = GL_DYNAMIC_DRAW;
    }

    glBindVertexArray(vertex_data.vao);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_data.vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, vertex_data.count * vertex_data.dimensions * sizeof(float), vertex_data.positions,
                 gl_usage);
    glVertexAttribPointer(0, vertex_data.dimensions, GL_FLOAT, GL_FALSE, vertex_data.dimensions * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    if (uvs != NULL)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_data.vbos[1]);
        glBufferData(GL_ARRAY_BUFFER, vertex_data.count * 2 * sizeof(float), vertex_data.uvs, gl_usage);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
    }

    if (colors != NULL)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_data.vbos[2]);
        glBufferData(GL_ARRAY_BUFFER, vertex_data.count * 4 * sizeof(float), vertex_data.colors, gl_usage);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(2);
    }

    if (instance_offsets != NULL)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_data.vbos[3]);
        glBufferData(GL_ARRAY_BUFFER, vertex_data.instance_offsets_count * vertex_data.dimensions * sizeof(float),
                     vertex_data.instance_offsets, gl_usage);
        glVertexAttribPointer(3, vertex_data.dimensions, GL_FLOAT, GL_FALSE, vertex_data.dimensions * sizeof(float),
                              (void *)0);
        glEnableVertexAttribArray(3);
        glVertexAttribDivisor(3, 1);
    }

    if (indices_count > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_data.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_data.indices_count * sizeof(float), vertex_data.indices,
                     GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    g_basic_vertex_data[g_basic_vertex_data_size] = vertex_data;
    return g_basic_vertex_data_size++;
}

void basic_update_vertex_data(unsigned int vertex_data_id, float *positions, float *uvs, float *colors,
                              unsigned int vertex_count)
{
    if (positions == NULL)
    {
        // TODO: logging stuff
        printf("Positions should not be null\n");
        return;
    }
    glBindBuffer(GL_ARRAY_BUFFER, g_basic_vertex_data[vertex_data_id].vbos[0]);
    if (vertex_count <= g_basic_vertex_data[vertex_data_id].count)
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_count * 2 * sizeof(float), positions);
    }
    else
    {
        glBufferData(GL_ARRAY_BUFFER, vertex_count * 2 * sizeof(float), positions, GL_DYNAMIC_DRAW);
    }
    if (uvs != NULL)
    {
        glBindBuffer(GL_ARRAY_BUFFER, g_basic_vertex_data[vertex_data_id].vbos[1]);
        if (vertex_count <= g_basic_vertex_data[vertex_data_id].count)
        {
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_count * 2 * sizeof(float), uvs);
        }
        else
        {
            glBufferData(GL_ARRAY_BUFFER, vertex_count * 2 * sizeof(float), uvs, GL_DYNAMIC_DRAW);
        }
    }
    if (colors != NULL)
    {
        glBindBuffer(GL_ARRAY_BUFFER, g_basic_vertex_data[vertex_data_id].vbos[2]);
        if (vertex_count <= g_basic_vertex_data[vertex_data_id].count)
        {
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_count * 4 * sizeof(float), colors);
        }
        else
        {
            glBufferData(GL_ARRAY_BUFFER, vertex_count * 4 * sizeof(float), colors, GL_DYNAMIC_DRAW);
        }
    }
    g_basic_vertex_data[vertex_data_id].count = vertex_count;
}

void basic_draw_arrays(unsigned int vertex_data_id, unsigned int program, unsigned int mode)
{
    glUseProgram(program);
    glBindVertexArray(g_basic_vertex_data[vertex_data_id].vao);
    glDrawArrays(mode, 0, g_basic_vertex_data[vertex_data_id].count);
}

void basic_draw_elements(unsigned int vertex_data_id, unsigned int program, unsigned int mode)
{
    glUseProgram(program);
    glBindVertexArray(g_basic_vertex_data[vertex_data_id].vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_basic_vertex_data[vertex_data_id].ebo);
    glDrawElements(mode, g_basic_vertex_data[vertex_data_id].indices_count, GL_UNSIGNED_INT, 0);
}

void basic_draw_arrays_instanced(unsigned int vertex_data_id, unsigned int program, int instance_count)
{
    glUseProgram(program);
    glBindVertexArray(g_basic_vertex_data[vertex_data_id].vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instance_count);
}

void clean_vertex_data(unsigned int vertex_data_id)
{
    glDeleteVertexArrays(1, &g_basic_vertex_data[vertex_data_id].vao);
    glDeleteBuffers(1, &g_basic_vertex_data[vertex_data_id].vbos[0]);
    glDeleteBuffers(1, &g_basic_vertex_data[vertex_data_id].vbos[1]);
    glDeleteBuffers(1, &g_basic_vertex_data[vertex_data_id].vbos[2]);
    glDeleteBuffers(1, &g_basic_vertex_data[vertex_data_id].ebo);
}

void clean_all_vertex_data()
{
    for (int i = 0; i < g_basic_vertex_data_size; i++)
    {
        clean_vertex_data(i);
    }
    g_basic_vertex_data_size = 0;
}
