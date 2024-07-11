//
// Created by sterling on 7/10/24.
//

#include "platform.h"
#include <malloc.h>
#include <memory.h>
#include <stdio.h>

char *read_file(const char *file_path, long *file_size)
{
    *file_size = 0;
    FILE *file = fopen(file_path, "rb");
    if (!file)
    {
        // TODO: logging stuff
        printf("Error opening file %s\n", file_path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(sizeof(char) * (*file_size + 1));
    if (!buffer)
    {
        printf("Error allocating memory\n");
        return NULL;
    }
    fread(buffer, sizeof(char), *file_size, file);
    fclose(file);

    return buffer;
}

int copy_file(const char *file_path, const char *output_file_path)
{
    long file_size = 0;
    char *data = read_file(file_path, &file_size);

    FILE *output_file = fopen(output_file_path, "wb");
    if (!output_file)
    {
        // TODO: logging stuff
        printf("Error opening output file %s\n", output_file_path);
        return -1;
    }

    unsigned long result = fwrite(data, sizeof(char), file_size, output_file);
    if (result != file_size)
    {
        // TODO: logging stuff
        printf("Error writing to output file %s\n", output_file_path);
        return -1;
    }

    fclose(output_file);
    free(data);
    return 0;
}
