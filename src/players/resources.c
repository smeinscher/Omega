//
// Created by sterling on 8/3/24.
//

#include "resources.h"
#include <malloc.h>
#include <memory.h>

Resources *resources_create(int player_count)
{
    Resources *resources = malloc(sizeof(Resources));
    if (resources == NULL)
    {
        printf("Error allocating resources\n");
        return NULL;
    }

    resources->radioactive_material_count = NULL;
    resources->rare_metals_count = NULL;
    resources->common_metals_count = NULL;

    resources->radioactive_material_count = malloc(sizeof(int) * player_count);
    if (resources->radioactive_material_count == NULL)
    {
        printf("Error allocating radioactive_material_count\n");
        resources_destroy(resources);
        return NULL;
    }
    memset(resources->radioactive_material_count, 0, sizeof(int) * player_count);
    resources->rare_metals_count = malloc(sizeof(int) * player_count);
    if (resources->rare_metals_count == NULL)
    {
        printf("Error allocating rare_metals_count\n");
        resources_destroy(resources);
        return NULL;
    }
    memset(resources->rare_metals_count, 0, sizeof(int) * player_count);
    resources->common_metals_count = malloc(sizeof(int) * player_count);
    if (resources->common_metals_count == NULL)
    {
        printf("Error allocating common_metals_count\n");
        resources_destroy(resources);
        return NULL;
    }
    memset(resources->common_metals_count, 0, sizeof(int) * player_count);

    return resources;
}

void resources_clear(Resources *resources)
{
    free(resources->radioactive_material_count);
    resources->radioactive_material_count = NULL;
    free(resources->rare_metals_count);
    resources->rare_metals_count = NULL;
    free(resources->common_metals_count);
    resources->common_metals_count = NULL;
}

void resources_destroy(Resources *resources)
{
    resources_clear(resources);
    free(resources);
}
