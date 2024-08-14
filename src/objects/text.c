//
// Created by sterling on 8/13/24.
//

#include "text.h"
#include "../renderer/opengl_freetype_wrapper.h"
#include <malloc.h>

static int g_next_text_id = 1;

TextData *text_data_create()
{
    TextData *text_data = malloc(sizeof(TextData));
    if (text_data == NULL)
    {
        printf("Error allocating text_data\n");
        return NULL;
    }
    da_string_init(&text_data->text, 1);
    da_float_init(&text_data->text_position, 2);
    da_float_init(&text_data->text_scale, 1);
    da_float_init(&text_data->text_color, 4);
    da_int_init(&text_data->text_id, 1);
    da_float_init(&text_data->text_time, 1);
    da_float_init(&text_data->text_lifetime, 1);

    return text_data;
}

void text_data_add(TextData *text_data, const char *text, int text_length, float x, float y, float scale, float r,
                   float g, float b, float a, float display_time)
{
    String str;
    string_init(&str, text_length);
    string_push_back(&str, text, text_length);
    da_string_push_back(&text_data->text, str);
    string_free(&str);
    da_float_push_back(&text_data->text_position, x);
    da_float_push_back(&text_data->text_position, y);
    da_float_push_back(&text_data->text_scale, scale);
    da_float_push_back(&text_data->text_color, r);
    da_float_push_back(&text_data->text_color, g);
    da_float_push_back(&text_data->text_color, b);
    da_float_push_back(&text_data->text_color, a);
    da_int_push_back(&text_data->text_id, g_next_text_id++);
    da_float_push_back(&text_data->text_time, 0.0f);
    da_float_push_back(&text_data->text_lifetime, display_time);
}

void text_data_remove(TextData *text_data, int text_index)
{
    da_string_remove(&text_data->text, text_index);
    da_float_remove(&text_data->text_position, text_index * 2);
    da_float_remove(&text_data->text_position, text_index * 2);
    da_float_remove(&text_data->text_scale, text_index);
    da_float_remove(&text_data->text_color, text_index * 4);
    da_float_remove(&text_data->text_color, text_index * 4);
    da_float_remove(&text_data->text_color, text_index * 4);
    da_float_remove(&text_data->text_color, text_index * 4);
    da_int_remove(&text_data->text_id, text_index);
    da_float_remove(&text_data->text_time, text_index);
    da_float_remove(&text_data->text_lifetime, text_index);
}

void text_data_update(TextData *text_data)
{
    for (int i = 0; i < text_data->text_lifetime.used; i++)
    {
        if (text_data->text_time.array[i] >= text_data->text_lifetime.array[i])
        {
            text_data_remove(text_data, i);
            i--;
            continue;
        }
        text_data->text_color.array[i * 4 + 3] =
            1.0f - text_data->text_time.array[i] / text_data->text_lifetime.array[i];
        // TODO: replace 0.05f with a variable
        text_data->text_time.array[i] += 0.05f;
    }
}

void text_data_render(TextData *text_data)
{
    for (int i = 0; i < text_data->text.used; i++)
    {
        opengl_freetype_wrapper_render_text(
            text_data->text.array[i].array, text_data->text.array[i].used, text_data->text_position.array[i * 2],
            text_data->text_position.array[i * 2 + 1], text_data->text_scale.array[i],
            text_data->text_color.array[i * 4], text_data->text_color.array[i * 4 + 1],
            text_data->text_color.array[i * 4 + 2], text_data->text_color.array[i * 4 + 3]);
    }
}

void text_data_clear(TextData *text_data)
{
    da_string_free(&text_data->text);
    da_float_free(&text_data->text_position);
    da_float_free(&text_data->text_scale);
    da_float_free(&text_data->text_color);
    da_int_free(&text_data->text_id);
    da_float_free(&text_data->text_time);
    da_float_free(&text_data->text_lifetime);
}

void text_data_destroy(TextData *text_data)
{
    text_data_clear(text_data);
    free(text_data);
}
