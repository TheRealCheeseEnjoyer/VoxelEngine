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

// sizes in chunk
#define WORLD_SIZE_X 10
#define WORLD_SIZE_Y 1
#define WORLD_SIZE_Z 10

#define MAX_RENDER_DISTANCE 12

#define NUMBER_OF_BLOCKS_IN_WORLD WORLD_SIZE_X * CHUNK_SIZE_X * WORLD_SIZE_Y * CHUNK_SIZE_Y * WORLD_SIZE_Z * CHUNK_SIZE_Z

#define LOADED_CHUNK_RANGE (int)(MAX_RENDER_DISTANCE * 1.5f)

class World {
public:
    Voxel* world = nullptr;
    // TODO std::vector is not thread safe, change it
    std::vector<Chunk> chunks;
    glm::vec<2, int> cameraChunkPos;
    std::queue<std::thread> threads;
    Shader* shader;
    siv::PerlinNoise::seed_type seed;
    siv::PerlinNoise noise;

    inline static World* instance = nullptr;

    explicit World(const Camera& camera);

    void createChunk(int x, int z);

    Chunk* getChunk(int x, int z);

    Voxel* getVoxel(int x, int y, int z);

    Voxel* getVoxel(glm::vec3 position);

    bool placeVoxel(int x, int y, int z, TextureType type);

    void destroyVoxel(int x, int y, int z);

    // TODO: generate new chunks that are now in range, and delete out of range chunks
    void updateLoadedChunks(const Camera& camera);

    glm::vec<2, int> getCameraChunkCoords(const Camera& camera);

    void draw(const Camera &camera);

    ~World();
};

#endif
