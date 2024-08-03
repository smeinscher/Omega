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

void da_int_init(DynamicIntArray *da, size_t initial_size);

void da_float_init(DynamicFloatArray *da, size_t initial_size);

void da_int_push_back(DynamicIntArray *da, int element);

void da_float_push_back(DynamicFloatArray *da, float element);

void da_int_insert(DynamicIntArray *da, int element, int index);

void da_float_insert(DynamicFloatArray *da, float element, int index);

void da_int_remove(DynamicIntArray *da, int index);

void da_float_remove(DynamicFloatArray *da, int index);

int da_int_pop_back(DynamicIntArray *da);

float da_float_pop_back(DynamicFloatArray *da);

int da_int_pop_front(DynamicIntArray *da);

float da_float_pop_front(DynamicFloatArray *da);

void da_int_clear(DynamicIntArray *da);

void da_float_clear(DynamicFloatArray *da);

void da_int_free(DynamicIntArray *da);

void da_float_free(DynamicFloatArray *da);

#endif // OMEGAAPP_DYNAMIC_ARRAY_H
