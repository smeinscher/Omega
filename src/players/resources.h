//
// Created by sterling on 8/3/24.
//

#ifndef OMEGAAPP_RESOURCES_H
#define OMEGAAPP_RESOURCES_H

enum ResourceType
{
    RADIOACTIVE_MATERIAL,
    RARE_METALS,
    COMMON_METALS,
    ENERGY
};

typedef struct Resources
{
    int *radioactive_material_count;
    int *rare_metals_count;
    int *common_metals_count;
    // TODO: implement energy
} Resources;

Resources *resources_create(int player_count);

void resources_clear(Resources *resources);

void resources_destroy(Resources *resources);

#endif // OMEGAAPP_RESOURCES_H
