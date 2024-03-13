#ifndef VOXELENGINE_VOXEL_H
#define VOXELENGINE_VOXEL_H

#include <glm/vec3.hpp>
#include <glad/glad.h>
#include "Shader.h"

struct Voxel {
public:
    inline static Shader* shader = nullptr;
    Voxel()  {
        if (shader == nullptr) {
            shader = new Shader();
        }
    }
};

#endif
