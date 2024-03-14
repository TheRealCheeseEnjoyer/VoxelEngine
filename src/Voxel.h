#ifndef VOXELENGINE_VOXEL_H
#define VOXELENGINE_VOXEL_H

#include <glm/vec3.hpp>
#include <glad/glad.h>
#include "Shader.h"
#include "TextureManager.h"

struct Voxel {
public:
    inline static Shader* shader = nullptr;
    TextureType type;
    explicit Voxel(TextureType type) : type(type)  {
        if (shader == nullptr) {
            shader = new Shader();
        }
    }
};

#endif
