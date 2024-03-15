#ifndef VOXELENGINE_WORLD_H
#define VOXELENGINE_WORLD_H

#include <cfloat>
#include <vector>
#include "Voxel.h"
#include "Chunk.h"
#include "Camera.h"

#define WORLD_SIZE_X 100
#define WORLD_SIZE_Z 100
#define MAX_RENDER_DISTANCE 12

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
                Chunk* north = getChunk(x, z + 1);
                Chunk* south = getChunk(x, z - 1);
                Chunk* east = getChunk(x - 1, z);
                Chunk* west = getChunk(x + 1, z);

                chunks[x][z]->init(north, south, east, west);
            }
        }
    }

    static Chunk* getChunk(int x, int z) {
        if (x < 0 || x >= WORLD_SIZE_X)
            return nullptr;

        if (z < 0 || z >= WORLD_SIZE_Z)
            return nullptr;

        return chunks[x][z];
    }

    static void draw(Camera camera) {
        glm::vec<2, int, glm::defaultp> cameraChunkCoords = {camera.Position.x / CHUNK_SIZE_X, camera.Position.z / CHUNK_SIZE_Z};
        for (int x = std::max(0, cameraChunkCoords.x - MAX_RENDER_DISTANCE); x < std::min(WORLD_SIZE_X, cameraChunkCoords.x + MAX_RENDER_DISTANCE); x++) {
            for (int z = std::max(0, cameraChunkCoords.y - MAX_RENDER_DISTANCE); z < std::min(WORLD_SIZE_Z, cameraChunkCoords.y + MAX_RENDER_DISTANCE); ++z) {
                chunks[x][z]->draw(camera.GetMatrices());
            }
        }
    }
};

#endif
