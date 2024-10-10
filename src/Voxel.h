#ifndef VOXELENGINE_VOXEL_H
#define VOXELENGINE_VOXEL_H

#include "TextureManager.h"

struct Voxel {
public:
    TextureType type;
    explicit Voxel(TextureType type) : type(type) {}
};

#endif
