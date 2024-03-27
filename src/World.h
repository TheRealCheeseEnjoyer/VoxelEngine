#ifndef VOXELENGINE_WORLD_H
#define VOXELENGINE_WORLD_H

#include <cfloat>
#include <vector>
#include <thread>
#include <queue>
#include "Voxel.h"
#include "Chunk.h"
#include "Camera.h"
#include "ThreadPool.h"

#define WORLD_SIZE_X 100
#define WORLD_SIZE_Z 100
#define MAX_RENDER_DISTANCE 12
#define SPAWN_CHUNK_RANGE (MAX_RENDER_DISTANCE * 2)

class World {
public:
    inline static Chunk* chunks[WORLD_SIZE_X][WORLD_SIZE_Z];

    inline static std::queue<std::thread> threads;
    inline static Shader* shader;

    World() = delete;

    static void init(const Camera& camera) {
        ThreadPool::Start();
        memset(chunks, 0, WORLD_SIZE_X * WORLD_SIZE_Z);
        shader = new Shader();
        auto start = glfwGetTime();

        srand(time(nullptr));
        siv::PerlinNoise::seed_type seed = rand() * UINT_MAX;
        siv::PerlinNoise noise{seed};

        for (int x = 0; x < WORLD_SIZE_X; ++x) {
            for (int z = 0; z < WORLD_SIZE_Z; ++z) {
                Chunk** north = getChunk(x, z + 1);
                Chunk** south = getChunk(x, z - 1);
                Chunk** east = getChunk(x - 1, z);
                Chunk** west = getChunk(x + 1, z);
                ThreadPool::QueueJob([x, z, noise, north, south, east, west]{World::chunks[x][z] = new Chunk({x, z}, shader, noise, north, south, east, west);});
            }
        }

        ThreadPool::waitForAllJobs();

        for (int x = WORLD_SIZE_X - 1; x >= 0; --x) {
            for (int z = WORLD_SIZE_Z - 1; z >= 0; --z) {
                ThreadPool::QueueJob([x, z]{World::chunks[x][z]->createMesh();});
            }
        }

        ThreadPool::waitForAllJobs();

        for (int x = WORLD_SIZE_X - 1; x >= 0; --x) {
            for (int z = WORLD_SIZE_Z - 1; z >= 0; --z) {
                chunks[x][z]->loadMesh();
            }
        }

        std::cout << glfwGetTime() - start << std::endl;
    }

    static Chunk** getChunk(int x, int z) {
        if (x < 0 || x >= WORLD_SIZE_X)
            return nullptr;

        if (z < 0 || z >= WORLD_SIZE_Z)
            return nullptr;

        return &chunks[x][z];
    }

    static void draw(Camera camera) {
        glm::vec<2, int, glm::defaultp> cameraChunkCoords = {camera.Position.x / CHUNK_SIZE_X, camera.Position.z / CHUNK_SIZE_Z};
        for (int x = std::max(0, cameraChunkCoords.x - MAX_RENDER_DISTANCE); x < std::min(WORLD_SIZE_X, cameraChunkCoords.x + MAX_RENDER_DISTANCE); x++) {
            for (int z = std::max(0, cameraChunkCoords.y - MAX_RENDER_DISTANCE); z < std::min(WORLD_SIZE_Z, cameraChunkCoords.y + MAX_RENDER_DISTANCE); ++z) {
                glm::vec<2, int, glm::defaultp> chunkCoords(x, z);
                if (glm::length((glm::vec2)(cameraChunkCoords - chunkCoords)) <= MAX_RENDER_DISTANCE)
                    chunks[x][z]->draw(camera.GetMatrices());
            }
        }
    }
};

#endif
