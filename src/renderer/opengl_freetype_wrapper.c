//
// Created by sterling on 8/13/24.
//

#include "opengl_freetype_wrapper.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../../external/glad/glad.h"
#include "opengl_renderer.h"

typedef struct Characters
{
    unsigned int texture_id[128];
    int size_x[128];
    int size_y[128];
    int bearing_left[128];
    int bearing_top[128];
    unsigned int advance[128];
} Characters;

static Characters characters;

static Shader shader = {0};

static unsigned int vao = 0;
static unsigned int vbo = 0;

bool opengl_freetype_wrapper_load_font(const char *font_path)
{
    shader = opengl_load_basic_shaders("../resources/shaders/text.vert", "../resources/shaders/text.frag");
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        printf("Error initializing FreeType\n");
        return false;
    }
    FT_Face face;
    if (FT_New_Face(ft, font_path, 0, &face))
    {
        printf("Error loading font\n");
        return false;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (int c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            printf("Failed to load glyph\n");
            return false;
        }
        glGenTextures(1, &characters.texture_id[c]);
        glBindTexture(GL_TEXTURE_2D, characters.texture_id[c]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED,
                     GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        characters.size_x[c] = face->glyph->bitmap.width;
        characters.size_y[c] = face->glyph->bitmap.rows;
        characters.bearing_left[c] = face->glyph->bitmap_left;
        characters.bearing_top[c] = face->glyph->bitmap_top;
        characters.advance[c] = (unsigned int)face->glyph->advance.x;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return true;
}

Shader *opengl_freetype_wrapper_get_font_shader()
{
    return &shader;
}

void opengl_freetype_wrapper_render_text(const char *text, int text_length, float x, float y, float scale, float r,
                                         float g, float b, float a)
{
    glUseProgram(shader.program);
    opengl_set_uniform_4f(shader.program, "text_color", r, g, b, a);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    for (int i = 0; i < text_length; i++)
    {
        if (text[i] == '\0')
        {
            break;
        }
        float x_pos = x + (float)characters.bearing_left[text[i]] * scale;
        float y_pos = y + (float)(characters.size_y[text[i]] - characters.bearing_top[text[i]]) * scale;
        float w = (float)characters.size_x[text[i]] * scale;
        float h = (float)characters.size_y[text[i]] * scale;

        float vertices[6][4] = {{x_pos, y_pos - h, 0.0f, 0.0f}, {x_pos, y_pos, 0.0f, 1.0f},
                                {x_pos + w, y_pos, 1.0f, 1.0f}, {x_pos, y_pos - h, 0.0f, 0.0f},
                                {x_pos + w, y_pos, 1.0f, 1.0f}, {x_pos + w, y_pos - h, 1.0f, 0.0f}};
        glBindTexture(GL_TEXTURE_2D, characters.texture_id[text[i]]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (float)(characters.advance[text[i]] >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
