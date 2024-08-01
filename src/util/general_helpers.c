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

void hex_grid_axial_to_offset(int q, int r, int *x, int *y)
{
    *x = q;
    *y = r + (q - (q & 1)) / 2;
}

void hex_grid_offset_to_axial(int x, int y, int *q, int *r)
{
    *q = x;
    *r = y - (x - (x & 1)) / 2;
}

void hex_grid_get_next(bool clockwise, int distance_from_center, int *q, int *r)
{
    int s = -(*q) - *r;
    if (distance_from_center > 1)
    {
        if (!clockwise)
        {
            if (*q > -distance_from_center && *r < 0 && s == distance_from_center)
            {
                (*q)--;
                (*r)++;
            }
            else if (*q == -distance_from_center && *r < distance_from_center && s > 0)
            {
                (*r)++;
            }
            else if (*q < 0 && *r == distance_from_center && s > -distance_from_center)
            {
                (*q)++;
            }
            else if (*q < distance_from_center && *r > 0 && s == -distance_from_center)
            {
                (*q)++;
                (*r)--;
            }
            else if (*q == distance_from_center && *r > -distance_from_center && s < 0)
            {
                (*r)--;
            }
            else
            {
                (*q)--;
            }
        }
        else
        {
            // TODO: implement this
            printf("Not implemented\n");
        }
    }
    else if (distance_from_center == 1)
    {
        if (!clockwise)
        {
            *q = *r;
            *r = s;
        }
        else
        {
            *r = *q;
            *q = s;
        }
    }
}
