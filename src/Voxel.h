#ifndef VOXELENGINE_VOXEL_H
#define VOXELENGINE_VOXEL_H

#include <glm/vec3.hpp>
#include <glad/glad.h>

struct Face {
    bool shouldDraw = true;
};

class Voxel {
public:
    glm::vec3 position;
    int faces = 0;
    short numberOfFaces = 0;

    Voxel() : Voxel(0, 0, 0) {}
    Voxel(float x, float y, float z) : position(x, y, z) {}
    explicit Voxel(glm::vec3 position) : position(position) {}

    void checkNeighbors() {

    }

    //TODO draw cubes selecting non occluded faces using EBOs
    void Draw() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faces);
        glDrawElements(GL_TRIANGLES, )
    }
};

#endif
