//
// Created by sterling on 7/11/24.
//

#ifndef FILE_HELPERS_H
#define FILE_HELPERS_H

char *read_file(const char *file_path, long *file_size);

int copy_file(const char *file_path, const char *output_file_path);

#endif //FILE_HELPERS_H
