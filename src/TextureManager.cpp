#include "../include/TextureManager.h"
#include <glad/glad.h>

void TextureManager::generateTexture(TextureType type) {
    unsigned int texture;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = nullptr;
    switch (type) {
        case TEXTURE_DEFAULT:
            data = stbi_load("assets/image.png", &width, &height, &nrChannels, 0);
        break;
        case TEXTURE_BEDROCK:
            data = stbi_load("assets/obamium.png", &width, &height, &nrChannels, 0);
        break;
        default:
            break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    textures.insert({type, texture});
}

unsigned int TextureManager::getTextureId(const TextureType type) {
    if (!textures.contains(type))
        generateTexture(type);

    return textures[type];
}
