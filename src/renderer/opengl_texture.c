//
// Created by sterling on 6/9/24.
//

#include "opengl_texture.h"

#include "../../external/glad/glad.h"
#include "../../external/stb/stb_image.h"

unsigned int generate_opengl_texture(const char *path)
{
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // stbi_set_flip_vertically_on_load(true);
    int nr_channels;
    int width, height;
    unsigned char *data = stbi_load(path, &width, &height, &nr_channels, STBI_rgb_alpha);
    if (data == NULL)
    {
        // TODO: logging stuff
        printf("Failed to load texture: %s", path);
        return 0;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return texture_id;
}