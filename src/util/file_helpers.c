//
// Created by sterling on 7/11/24.
//

#include "file_helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

char *read_file(const char *file_path, long *file_size)
{
    *file_size = 0;
    FILE *file = fopen(file_path, "rb");
    if (file == NULL)
    {
        // TODO: logging stuff
        printf("Error opening file %s\n", file_path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(sizeof(char) * (*file_size + 1));
    if (buffer == NULL)
    {
        printf("Error allocating memory\n");
        return NULL;
    }
    fread(buffer, sizeof(char), *file_size, file);
    fclose(file);

    return buffer;
}

int write_file(const char *file_path, const char *data, const unsigned long size)
{
    FILE *output_file = fopen(file_path, "wb");
    if (output_file == NULL)
    {
        // TODO: logging stuff
        printf("Error opening output file %s\n", file_path);
        return -1;
    }

    unsigned long result = fwrite(data, sizeof(char), size, output_file);
    fclose(output_file);
    if (result != size)
    {
        printf("Error writing to output file %s\n", file_path);
        return -1;
    }
    return 0;
}

int copy_file(const char *file_path, const char *output_file_path)
{
    long file_size = 0;
    char *data = read_file(file_path, &file_size);

    FILE *output_file = fopen(output_file_path, "wb");
    if (output_file == NULL)
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

struct timespec get_file_timestamp(const char *path)
{
    struct stat st = {0};
    int ierr = stat(path, &st);
    if (ierr != 0)
    {
        printf("Error loading file timestamp\n");
    }
    return st.st_mtim;
}
