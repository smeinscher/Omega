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

void string_init(String *str, size_t initial_size)
{
    str->used = 0;
    str->array = malloc(sizeof(char) * initial_size);
    if (str->array == NULL)
    {
        printf("Error allocating dynamic char array (String)\n");
        str->size = 0;
        return;
    }
    str->size = initial_size;
}

void da_string_init(DynamicStringArray *da, size_t initial_size)
{
    da->used = 0;
    da->array = malloc(sizeof(String) * initial_size);
    if (da->array == NULL)
    {
        printf("Error allocating dynamic string array\n");
        da->size = 0;
        return;
    }
    for (int i = 0; i < initial_size; i++)
    {
        da->array[i].array = NULL;
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

bool string_realloc(String *str)
{
    str->size *= 2;
    char *temp = realloc(str->array, str->size * sizeof(char));
    if (temp == NULL)
    {
        printf("Error reallocating memory for string\n");
        return false;
    }
    str->array = temp;
    return true;
}

bool da_string_realloc(DynamicStringArray *da)
{
    da->size *= 2;
    String *temp = realloc(da->array, da->size * sizeof(String));
    if (temp == NULL)
    {
        printf("Error reallocating memory for dynamic string array\n");
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
        if (!da_float_realloc(da))
        {
            return;
        }
    }
    da->array[da->used++] = element;
}

void string_push_back(String *str, const char *characters, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (str->used >= str->size)
        {
            if (!string_realloc(str))
            {
                return;
            }
        }
        str->array[str->used++] = characters[i];
    }
}

void da_string_push_back(DynamicStringArray *da, String element)
{
    if (da->used >= da->size)
    {
        if (!da_string_realloc(da))
        {
            return;
        }
    }
    string_init(&da->array[da->used], element.size);
    string_push_back(&da->array[da->used], element.array, element.used);
    da->used++;
}

void da_int_insert(DynamicIntArray *da, int element, int index)
{
    printf("Please fix\n");
    return;
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
    printf("Please fix\n");
    return;
    if (index >= da->used)
    {
        printf("Dynamic float array insert index out of bounds\n");
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

void string_insert(String *str, const char *element, int length, int index)
{
    printf("Please fix\n");
    return;
    if (index >= str->used)
    {
        printf("String array insert index out of bounds\n");
        return;
    }
    int temp_index = 0;
    for (int i = 0; i < str->used; i++)
    {
        if (temp_index >= str->size - 1)
        {
            if (!string_realloc(str))
            {
                return;
            }
        }
        str->array[temp_index] = str->array[i];
        temp_index++;
        if (temp_index == index)
        {
            char *displaced_chars = malloc(sizeof(char) * length);
            for (int j = 0; j < length; j++)
            {
                if (temp_index >= str->size - 1)
                {
                    if (!string_realloc(str))
                    {
                        return;
                    }
                }
                displaced_chars[j] = str->array[temp_index];
                str->array[temp_index] = element[j];
                temp_index++;
            }

            free(displaced_chars);
        }
    }
}

void da_string_insert(DynamicStringArray *da, String element, int index)
{
    printf("Please fix\n");
    return;
    if (index >= da->used)
    {
        printf("Dynamic float array insert index out of bounds\n");
        return;
    }
    int temp_index = 0;
    for (int i = 0; i < da->used; i++)
    {
        if (temp_index >= da->size - 1)
        {
            if (!da_string_realloc(da))
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

void string_remove(String *str, int index)
{
    printf("Please implement\n");
}

void da_string_remove(DynamicStringArray *da, int index)
{
    if (index >= da->used)
    {
        printf("Dynamic string array remove index out of bounds\n");
        return;
    }
    int temp_index = 0;
    for (int i = 0; i < da->used; i++)
    {
        if (i == index)
        {
            string_free(&da->array[i]);
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

String da_string_pop_back(DynamicStringArray *da)
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

String da_string_pop_front(DynamicStringArray *da)
{
    String temp = da->array[0];
    da_string_remove(da, 0);
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

void string_clear(String *str)
{
    memset(str->array, 0, sizeof(char) * str->used);
    str->used = 0;
}

void da_string_clear(DynamicStringArray *da)
{
    for (int i = 0; i < da->used; i++)
    {
        string_free(&da->array[i]);
        da->array[i].array = NULL;
    }
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

void string_free(String *str)
{
    free(str->array);
    str->array = NULL;
    str->used = 0;
    str->size = 0;
}

void da_string_free(DynamicStringArray *da)
{
    for (int i = 0; i < da->used; i++)
    {
        string_free(&da->array[i]);
    }
    free(da->array);
    da->used = 0;
    da->size = 0;
}

char *string_to_c_str(String *str)
{
    char *temp = malloc(sizeof(char) * str->used + 1);
    for (int i = 0; i < str->used; i++)
    {
        temp[i] = str->array[i];
    }
    temp[str->used] = '\0';
    return temp;
}
