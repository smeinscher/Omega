//
// Created by sterling on 8/8/24.
//

#include "star_background.h"
#include "../renderer/camera.h"
#include <malloc.h>

StarBackground *star_background_create()
{
    StarBackground *star_background = malloc(sizeof(StarBackground));
    if (star_background == NULL)
    {
        printf("Failed to allocate star_background\n");
        return NULL;
    }

    star_background->star_background_positions = NULL;
    star_background->star_background_colors = NULL;

    star_background->star_background_positions = malloc(sizeof(float) * 12);
    if (star_background->star_background_positions == NULL)
    {
        printf("Failed to allocate star_background_positions\n");
        free(star_background);
        return NULL;
    }
    float max_x = (float)camera_get_viewport_width();
    float max_y = (float)camera_get_viewport_height();

    star_background->star_background_positions[0] = -1.0f;
    star_background->star_background_positions[1] = 1.0f;
    star_background->star_background_positions[2] = 1.0f;
    star_background->star_background_positions[3] = -1.0f;
    star_background->star_background_positions[4] = -1.0f;
    star_background->star_background_positions[5] = -1.0f;
    star_background->star_background_positions[6] = -1.0f;
    star_background->star_background_positions[7] = 1.0f;
    star_background->star_background_positions[8] = 1.0f;
    star_background->star_background_positions[9] = -1.0f;
    star_background->star_background_positions[10] = 1.0f;
    star_background->star_background_positions[11] = 1.0f;

    star_background->star_background_colors = malloc(sizeof(float) * 24);
    if (star_background->star_background_colors == NULL)
    {
        printf("Failed to allocate star_background_colors\n");
        free(star_background->star_background_positions);
        free(star_background);
        return NULL;
    }

    for (int i = 0; i < 6; i++)
    {
        star_background->star_background_colors[i * 4] = 1.0f;
        star_background->star_background_colors[i * 4 + 1] = 1.0f;
        star_background->star_background_colors[i * 4 + 2] = 1.0f;
        star_background->star_background_colors[i * 4 + 3] = 1.0f;
    }

    return star_background;
}
