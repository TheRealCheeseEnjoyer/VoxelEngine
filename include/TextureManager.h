#ifndef VOXELENGINE_TEXTUREMANAGER_H
#define VOXELENGINE_TEXTUREMANAGER_H

#include <unordered_map>
#include "../include/stb_image.h"

enum TextureType {
    TEXTURE_NONE,
    TEXTURE_DEFAULT,
    TEXTURE_BEDROCK,
};

class TextureManager {
private:
    inline static std::unordered_map<TextureType, unsigned int> textures;
    inline static std::unordered_map<TextureType, std::tuple<float, float>> offsetsAndLengths;

    static void generateTexture(TextureType type);
public:
    static unsigned int getTextureId(TextureType type);
};

#endif
