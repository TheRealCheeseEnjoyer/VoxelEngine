#ifndef VOXELENGINE_VOXEL_H
#define VOXELENGINE_VOXEL_H

#include <glm/vec3.hpp>
#include <glad/glad.h>
#include "Shader.h"
#include "TextureManager.h"

struct Voxel {
public:
    TextureType type;
    explicit Voxel(TextureType type) : type(type) {}
};

#endif
