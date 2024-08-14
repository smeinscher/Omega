//
// Created by sterling on 8/13/24.
//

#ifndef OMEGAAPP_TEXT_H
#define OMEGAAPP_TEXT_H

#include "../util/dynamic_array.h"
typedef struct TextData
{
    DynamicStringArray text;
    DynamicFloatArray text_position;
    DynamicFloatArray text_scale;
    DynamicFloatArray text_color;
    DynamicIntArray text_id;
    DynamicFloatArray text_time;
    DynamicFloatArray text_lifetime;
} TextData;

TextData *text_data_create();

void text_data_add(TextData *text_data, const char *text, int text_length, float x, float y, float scale, float r,
                   float g, float b, float a, float display_time);

void text_data_remove(TextData *text_data, int text_index);

void text_data_update(TextData *text_data);

void text_data_render(TextData *text_data);

void text_animate_position(TextData *text_data, int text_id, float start_x, float start_y, float end_x, float end_y,
                           float t);

void text_animate_scale(TextData *text_data, int text_id, float start_scale, float end_scale, float t);

void text_animate_color(TextData *text_data, int text_id, float start_r, float start_g, float start_b, float start_a,
                        float end_r, float end_g, float end_b, float end_a, float t);

void text_data_clear(TextData *text_data);

void text_data_destroy(TextData *text_data);

#endif // OMEGAAPP_TEXT_H
