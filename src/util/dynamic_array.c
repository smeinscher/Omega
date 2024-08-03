//
// Created by sterling on 7/28/24.
//

#include "dynamic_array.h"
#include <memory.h>

void da_int_init(DynamicIntArray *da, size_t initial_size)
{
    da->used = 0;
    da->array = malloc(sizeof(int) * initial_size);
    if (da->array == NULL)
    {
        printf("Error allocating dynamic int array\n");
        da->size = 0;
        return;
    }
    da->size = initial_size;
}

void da_float_init(DynamicFloatArray *da, size_t initial_size)
{
    da->used = 0;
    da->array = malloc(sizeof(float) * initial_size);
    if (da->array == NULL)
    {
        printf("Error allocating dynamic float array\n");
        da->size = 0;
        return;
    }
    da->size = initial_size;
}

bool da_int_realloc(DynamicIntArray *da)
{
    da->size *= 2;
    int *temp = realloc(da->array, da->size * sizeof(int));
    if (temp == NULL)
    {
        printf("Error reallocating memory for dynamic int array\n");
        return false;
    }
    da->array = temp;
    return true;
}

bool da_float_realloc(DynamicFloatArray *da)
{
    da->size *= 2;
    float *temp = realloc(da->array, da->size * sizeof(float));
    if (temp == NULL)
    {
        printf("Error reallocating memory for dynamic float array\n");
        return false;
    }
    da->array = temp;
    return true;
}

void da_int_push_back(DynamicIntArray *da, int element)
{
    if (da->used >= da->size)
    {
        if (!da_int_realloc(da))
        {
            return;
        }
    }
    da->array[da->used++] = element;
}

void da_float_push_back(DynamicFloatArray *da, float element)
{
    if (da->used >= da->size)
    {
        if (da_float_realloc(da))
        {
            return;
        }
    }
    da->array[da->used++] = element;
}

void da_int_insert(DynamicIntArray *da, int element, int index)
{
    if (index >= da->used)
    {
        printf("Dynamic int array insert index out of bounds\n");
        return;
    }
    int temp_index = 0;
    for (int i = 0; i < da->used; i++)
    {
        if (temp_index >= da->size - 1)
        {
            if (!da_int_realloc(da))
            {
                return;
            }
        }
        da->array[temp_index] = da->array[i];
        temp_index++;
        if (temp_index == index)
        {
            da->array[temp_index] = element;
            temp_index++;
        }
    }
    da->used++;
}

void da_float_insert(DynamicFloatArray *da, float element, int index)
{
    if (index >= da->used)
    {
        printf("Dynamic int array insert index out of bounds\n");
        return;
    }
    int temp_index = 0;
    for (int i = 0; i < da->used; i++)
    {
        if (temp_index >= da->size - 1)
        {
            if (!da_float_realloc(da))
            {
                return;
            }
        }
        da->array[temp_index] = da->array[i];
        temp_index++;
        if (temp_index == index)
        {
            da->array[temp_index] = element;
            temp_index++;
        }
    }
    da->used++;
}

void da_int_remove(DynamicIntArray *da, int index)
{
    if (index >= da->used)
    {
        printf("Dynamic int array remove index out of bounds\n");
        return;
    }
    int temp_index = 0;
    for (int i = 0; i < da->used; i++)
    {
        if (i == index)
        {
            continue;
        }
        da->array[temp_index] = da->array[i];
        temp_index++;
    }
    da->used--;
}

void da_float_remove(DynamicFloatArray *da, int index)
{
    if (index >= da->used)
    {
        printf("Dynamic float array remove index out of bounds\n");
        return;
    }
    int temp_index = 0;
    for (int i = 0; i < da->used; i++)
    {
        if (i == index)
        {
            continue;
        }
        da->array[temp_index] = da->array[i];
        temp_index++;
    }
    da->used--;
}

int da_int_pop_back(DynamicIntArray *da)
{
    da->used--;
    return da->array[da->used];
}

float da_float_pop_back(DynamicFloatArray *da)
{
    da->used--;
    return da->array[da->used];
}

int da_int_pop_front(DynamicIntArray *da)
{
    int temp = da->array[0];
    da_int_remove(da, 0);
    return temp;
}

float da_float_pop_front(DynamicFloatArray *da)
{
    float temp = da->array[0];
    da_float_remove(da, 0);
    return temp;
}

void da_int_clear(DynamicIntArray *da)
{
    memset(da->array, 0, sizeof(int) * da->used);
    da->used = 0;
}

void da_float_clear(DynamicFloatArray *da)
{
    memset(da->array, 0, sizeof(float) * da->used);
    da->used = 0;
}

void da_int_free(DynamicIntArray *da)
{
    if (da == NULL)
    {
        return;
    }
    free(da->array);
    da->array = NULL;
    da->used = 0;
    da->size = 0;
}

void da_float_free(DynamicFloatArray *da)
{
    free(da->array);
    da->array = NULL;
    da->used = 0;
    da->size = 0;
}
