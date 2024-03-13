#ifndef VOXELENGINE_WORLD_H
#define VOXELENGINE_WORLD_H

#include <cfloat>
#include <vector>
#include "Voxel.h"
#include "Chunk.h"


class World {
public:
    std::vector<std::vector<Chunk*>> chunks;

    World() {
        for (int x = 0; x < 1; ++x) {
            chunks.emplace_back();
            for (int y = 0; y < 1; ++y) {
                chunks[x].push_back(new Chunk({x, 0, y}));
            }
        }
    }

    void draw(glm::mat4 viewMatrix,glm::mat4 projection) {
        for (auto & chunk : chunks) {
            for (auto & j : chunk) {
                j->draw(viewMatrix, projection);
            }
        }
    }
};

#endif
