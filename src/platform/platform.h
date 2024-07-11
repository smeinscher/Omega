//
// Created by sterling on 7/10/24.
//

#ifndef OMEGAAPP_PLATFORM_H
#define OMEGAAPP_PLATFORM_H

char *read_file(const char *file_path, long *file_size);

int copy_file(const char *file_path, const char *output_file_path);

#endif // OMEGAAPP_PLATFORM_H
