#ifndef VOXELENGINE_VOXEL_H
#define VOXELENGINE_VOXEL_H

#include <glm/vec3.hpp>

struct Face {
    bool shouldDraw = true;
};

class Voxel {
public:
    glm::vec3 position;

    Voxel() : Voxel(0, 0, 0) {}
    Voxel(float x, float y, float z) : position(x, y, z) {}
    explicit Voxel(glm::vec3 position) : position(position) {}

    //TODO draw cubes selecting non occluded faces using EBOs
    void Draw() {

    }
};

#endif
