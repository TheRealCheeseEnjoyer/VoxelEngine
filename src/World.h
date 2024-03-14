#ifndef VOXELENGINE_WORLD_H
#define VOXELENGINE_WORLD_H

#include <cfloat>
#include <vector>
#include "Voxel.h"
#include "Chunk.h"

#define WORLD_SIZE_X 100
#define WORLD_SIZE_Z 100

class World {
public:
    inline static std::vector<std::vector<Chunk*>> chunks;

    World() = delete;

    static void init() {
        for (int x = 0; x < WORLD_SIZE_X; ++x) {
            chunks.emplace_back();
            for (int z = 0; z < WORLD_SIZE_Z; ++z) {
                chunks[x].push_back(new Chunk({x, 0, z}));
            }
        }

        for (int x = 0; x < WORLD_SIZE_X; ++x) {
            for (int z = 0; z < WORLD_SIZE_Z; ++z) {
                Chunk* north = z + 1 >= WORLD_SIZE_Z ? nullptr : chunks[x][z + 1];
                Chunk* south = z - 1 < 0 ? nullptr : chunks[x][z - 1];
                Chunk* east = x - 1 < 0 ? nullptr : chunks[x - 1][z];
                Chunk* west = x + 1 >= WORLD_SIZE_X ? nullptr : chunks[x + 1][z];

                chunks[x][z]->init(north, south, east, west);
            }
        }
    }

    static Chunk* getChunk(int x, int z) {
        if (x < 0 || x >= chunks.size())
            return nullptr;

        if (z < 0 || z >= chunks[x].size())
            return nullptr;

        return chunks[x][z];
    }

    static void draw(glm::mat4 matrices) {
        for (auto & chunk : chunks) {
            for (auto & j : chunk) {
                j->draw(matrices);
            }
        }
    }
};

#endif
