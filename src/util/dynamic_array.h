//
// Created by sterling on 7/28/24.
//

#ifndef OMEGAAPP_DYNAMIC_ARRAY_H
#define OMEGAAPP_DYNAMIC_ARRAY_H

#include <malloc.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct DynamicIntArray
{
    int *array;
    size_t used;
    size_t size;
} DynamicIntArray;

typedef struct DynamicFloatArray
{
    float *array;
    size_t used;
    size_t size;
} DynamicFloatArray;

typedef struct DynamicCharArray
{
    char *array;
    size_t used;
    size_t size;
} DynamicCharArray;

typedef DynamicCharArray String;

typedef struct DynamicStringArray
{
    String *array;
    size_t used;
    size_t size;
} DynamicStringArray;

void da_int_init(DynamicIntArray *da, size_t initial_size);

void da_float_init(DynamicFloatArray *da, size_t initial_size);

void string_init(String *str, size_t initial_size);

void da_string_init(DynamicStringArray *da, size_t initial_size);

void da_int_push_back(DynamicIntArray *da, int element);

void da_float_push_back(DynamicFloatArray *da, float element);

void string_push_back(String *str, const char *characters, int length);

void da_string_push_back(DynamicStringArray *da, String element);

void da_int_insert(DynamicIntArray *da, int element, int index);

void da_float_insert(DynamicFloatArray *da, float element, int index);

void string_insert(String *str, const char *element, int length, int index);

void da_string_insert(DynamicStringArray *da, String element, int index);

void da_int_remove(DynamicIntArray *da, int index);

void da_float_remove(DynamicFloatArray *da, int index);

void string_remove(String *str, int index);

void da_string_remove(DynamicStringArray *da, int index);

int da_int_pop_back(DynamicIntArray *da);

float da_float_pop_back(DynamicFloatArray *da);

String da_string_pop_back(DynamicStringArray *da);

int da_int_pop_front(DynamicIntArray *da);

float da_float_pop_front(DynamicFloatArray *da);

String da_string_pop_front(DynamicStringArray *da);

void da_int_clear(DynamicIntArray *da);

void da_float_clear(DynamicFloatArray *da);

void string_clear(String *str);

void da_string_clear(DynamicStringArray *da);

void da_int_free(DynamicIntArray *da);

void da_float_free(DynamicFloatArray *da);

void string_free(String *str);

void da_string_free(DynamicStringArray *da);

char *string_to_c_str(String *str);

#endif // OMEGAAPP_DYNAMIC_ARRAY_H
