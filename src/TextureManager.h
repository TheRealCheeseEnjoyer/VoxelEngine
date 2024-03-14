#ifndef VOXELENGINE_TEXTUREMANAGER_H
#define VOXELENGINE_TEXTUREMANAGER_H

#include <vector>
#include <iostream>
#include <unordered_map>
#include <glad/glad.h>
#include "stb_image.h"

enum TextureType {
    TEXTURE_DEFAULT,
    TEXTURE_BEDROCK
};



class TextureManager {
private:
    inline static std::unordered_map<TextureType, unsigned int> textures;
    inline static std::unordered_map<TextureType, std::tuple<float, float>> offsetsAndLengths;

    static void generateTexture(TextureType type) {
        unsigned int texture;
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        stbi_set_flip_vertically_on_load(true);
        int width, height, nrChannels;
        unsigned char* data = stbi_load("assets/spritesheet.png", &width, &height, &nrChannels, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);

        textures.insert({type, texture});
        switch (type) {
            case TEXTURE_DEFAULT:
                offsetsAndLengths.insert({type, {0.f, 273/785.f}});
                break;
            case TEXTURE_BEDROCK:
                offsetsAndLengths.insert({type, {273.f/785.f, 1}});
                break;
        }

    }
public:
    static std::tuple<float, float> getOffset(TextureType type) {
        if (!offsetsAndLengths.contains(type)) {
            generateTexture(type);
        }

        return offsetsAndLengths[type];
    }

    static unsigned int getTextureId(TextureType type) {
        if (!textures.contains(type))
            generateTexture(type);

        return textures[type];
    }
};

#endif
