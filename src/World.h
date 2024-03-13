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
        for (int i = 0; i < chunks.size(); ++i) {
            for (int j = 0; j < chunks[i].size(); ++j) {
                chunks[i][j]->draw(viewMatrix, projection);
            }
        }
    }
};

#endif
