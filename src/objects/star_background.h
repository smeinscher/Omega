//
// Created by sterling on 8/8/24.
//

#ifndef OMEGAAPP_STAR_BACKGROUND_H
#define OMEGAAPP_STAR_BACKGROUND_H

typedef struct StarBackground
{
    float *star_background_positions;
    float *star_background_colors;

} StarBackground;

StarBackground *star_background_create();

#endif // OMEGAAPP_STAR_BACKGROUND_H
