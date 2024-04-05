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
#define LOADED_CHUNK_RANGE (int)(MAX_RENDER_DISTANCE * 1.5f)

class World {
public:
    inline static Chunk* chunks[WORLD_SIZE_X][WORLD_SIZE_Z];
    inline static glm::vec<2, int, glm::defaultp> cameraChunkPos;
    inline static std::queue<std::thread> threads;
    inline static Shader* shader;
    inline static siv::PerlinNoise::seed_type seed;
    inline static siv::PerlinNoise* noise = nullptr;

    World() = delete;

    static void createChunk(int x, int z) {
        Chunk** north = getChunk(x, z + 1);
        Chunk** south = getChunk(x, z - 1);
        Chunk** east = getChunk(x - 1, z);
        Chunk** west = getChunk(x + 1, z);
        ThreadPool::QueueJob([x, z, north, south, east, west]{World::chunks[x][z] = new Chunk({x, z}, shader, *noise, north, south, east, west);});
    }

    static void init(const Camera& camera) {
        ThreadPool::Start();
        memset(chunks, 0, WORLD_SIZE_X * WORLD_SIZE_Z);
        shader = new Shader();
        auto start = glfwGetTime();

        srand(time(nullptr));
        seed = rand() * UINT_MAX;
        noise = new siv::PerlinNoise(seed);

        cameraChunkPos = getCameraChunkCoords(camera);
        std::cout << cameraChunkPos.x << " " << cameraChunkPos.y << std::endl;

        int xStart = std::max(0, cameraChunkPos.x - LOADED_CHUNK_RANGE), xEnd = std::min(WORLD_SIZE_X, cameraChunkPos.x + LOADED_CHUNK_RANGE);
        int zStart = std::max(0, cameraChunkPos.y - LOADED_CHUNK_RANGE), zEnd = std::min(WORLD_SIZE_Z, cameraChunkPos.y + LOADED_CHUNK_RANGE);

        for (int x = xStart; x < xEnd; ++x) {
            for (int z = zStart; z < zEnd; ++z) {
                createChunk(x, z);
            }
        }

        ThreadPool::waitForAllJobs();

        for (int x = xStart; x < xEnd; x++) {
            for (int z = zStart; z < zEnd; z++) {
                ThreadPool::QueueJob([x, z]{World::chunks[x][z]->createMesh();});
            }
        }

        ThreadPool::waitForAllJobs();

        for (int x = xStart; x < xEnd; x++) {
            for (int z = zStart; z < zEnd; z++) {
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

    // TODO: generate new chunks that are now in range, and delete out of range chunks
    static void updateLoadedChunks(const Camera& camera) {
        if (cameraChunkPos == getCameraChunkCoords(camera))
            return;

        std::vector<Chunk**> newChunks;
        newChunks.reserve(4 * LOADED_CHUNK_RANGE - 1);
        cameraChunkPos = getCameraChunkCoords(camera);

        int xStart = std::max(0, cameraChunkPos.x - LOADED_CHUNK_RANGE), xEnd = std::min(WORLD_SIZE_X, cameraChunkPos.x + LOADED_CHUNK_RANGE);
        int zStart = std::max(0, cameraChunkPos.y - LOADED_CHUNK_RANGE), zEnd = std::min(WORLD_SIZE_Z, cameraChunkPos.y + LOADED_CHUNK_RANGE);

        for (int x = xStart; x < xEnd; ++x) {
            for (int z = zStart; z < zEnd; ++z) {
                if (chunks[x][z] == nullptr) {
                    createChunk(x, z);
                    newChunks.push_back(&(chunks[x][z]));
                }
            }
        }

        ThreadPool::waitForAllJobs();

        for (auto newChunk : newChunks) {
            ThreadPool::QueueJob([newChunk] {(*newChunk)->createMesh();});
        }

        ThreadPool::waitForAllJobs();

        for (auto newChunk : newChunks) {
            (*newChunk)->loadMesh();
        }

    }

    static glm::vec<2, int, glm::defaultp> getCameraChunkCoords(const Camera& camera) {
        return {camera.Position.x / CHUNK_SIZE_X, camera.Position.z / CHUNK_SIZE_Z};
    }

    static void draw(Camera camera) {
        auto cameraChunkCoords = getCameraChunkCoords(camera);
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
