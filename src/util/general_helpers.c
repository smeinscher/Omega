//
// Created by sterling on 7/29/24.
//

#include "general_helpers.h"
#include <malloc.h>
#include <stdio.h>

void realloc_int(int **int_ptr, size_t size)
{
    int *temp_int_ptr = realloc(*int_ptr, size * sizeof(int));
    if (temp_int_ptr == NULL)
    {
        printf("Error reallocating memory for board_unit_owner\n");
        return;
    }
    *int_ptr = temp_int_ptr;
}

void realloc_float(float **float_ptr, size_t size)
{
    float *temp_float_ptr = realloc(*float_ptr, size * sizeof(float));
    if (temp_float_ptr == NULL)
    {
        printf("Error reallocating memory for board_unit_owner\n");
        return;
    }
    *float_ptr = temp_float_ptr;
}
