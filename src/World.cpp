#include "../include/World.h"

World::World(const Camera &camera) {
    instance = this;
    shader = new Shader();
    chunks.reserve(WORLD_SIZE_X * WORLD_SIZE_Y * WORLD_SIZE_Z);
    world = (Voxel*)calloc(NUMBER_OF_BLOCKS_IN_WORLD, sizeof(Voxel));

    auto start = glfwGetTime();

    srand(time(nullptr));
    seed = rand() * UINT_MAX;
    noise.reseed(seed);
    cameraChunkPos = getCameraChunkCoords(camera);

    int xStart = std::max(0, cameraChunkPos.x - LOADED_CHUNK_RANGE), xEnd = std::min(WORLD_SIZE_X, cameraChunkPos.x + LOADED_CHUNK_RANGE);
    int zStart = std::max(0, cameraChunkPos.y - LOADED_CHUNK_RANGE), zEnd = std::min(WORLD_SIZE_Z, cameraChunkPos.y + LOADED_CHUNK_RANGE);

    for (int x = xStart; x < xEnd; ++x) {
        for (int z = zStart; z < zEnd; ++z) {
            createChunk(x, z);
        }
    }

    ThreadPool::waitForAllJobs();

    for (auto& chunk : chunks) {
        chunk.createMesh();
    }

    /*for (int x = xStart; x < xEnd; x++) {
        for (int z = zStart; z < zEnd; z++) {
            //ThreadPool::QueueJob([x, z, this]{chunks[x * WORLD_SIZE_X + z].createMesh();});
            if (!tChunks.contains(&chunks[x * WORLD_SIZE_Z + z]))
                fprintf(stderr, "Chunk error");
            chunks[x * WORLD_SIZE_Z + z].createMesh();
        }
    }*/

    ThreadPool::waitForAllJobs();

    for (auto& chunk : chunks) {
        chunk.loadMesh();
    }

    /*for (int x = xStart; x < xEnd; x++) {
        for (int z = zStart; z < zEnd; z++) {
            chunks[x * WORLD_SIZE_Z + z].loadMesh();
        }
    }*/

    std::cout << glfwGetTime() - start << std::endl;
}

void World::createChunk(int x, int z) {
    Chunk* north = getChunk(x, z + 1);
    Chunk* south = getChunk(x, z - 1);
    Chunk* east = getChunk(x - 1, z);
    Chunk* west = getChunk(x + 1, z);
    chunks.emplace_back(glm::vec2(x, z), shader, noise, north, south, east, west, &world[x * WORLD_SIZE_X * CHUNK_SIZE_X * CHUNK_SIZE_Z * CHUNK_SIZE_Y + z * CHUNK_SIZE_Z * CHUNK_SIZE_X * CHUNK_SIZE_Y]);
    //chunks[x][z].init({x, z}, shader, *noise, north, south, east, west);
    //ThreadPool::QueueJob([x, z, north, south, east, west, this]{chunks[x * WORLD_SIZE_X + z].init({x, z}, shader, *noise, north, south, east, west, &world[x * WORLD_SIZE_X * CHUNK_SIZE_X * CHUNK_SIZE_Z * CHUNK_SIZE_Y + z * CHUNK_SIZE_Z * CHUNK_SIZE_X * CHUNK_SIZE_Y]);});
    // ThreadPool::QueueJob([x, z, north, south, east, west, this] {
    //     chunks.emplace_back(glm::vec2(x, z), shader, *noise, north, south, east, west, &world[x * WORLD_SIZE_X * CHUNK_SIZE_X * CHUNK_SIZE_Z * CHUNK_SIZE_Y + z * CHUNK_SIZE_Z * CHUNK_SIZE_X * CHUNK_SIZE_Y]);
    // });
    //ThreadPool::QueueJob([x, z, north, south, east, west]{World::chunks[x][z] = new Chunk({x, z}, shader, *noise, north, south, east, west);});
}

Chunk * World::getChunk(int x, int z) {
    if (x < 0 || x >= WORLD_SIZE_X)
        return nullptr;

    if (z < 0 || z >= WORLD_SIZE_Z)
        return nullptr;

    return &chunks[x * WORLD_SIZE_Z + z];
}

Voxel * World::getVoxel(int x, int y, int z) {
    if (x < 0 || x >= WORLD_SIZE_X * CHUNK_SIZE_X || y < 0 || y >= WORLD_SIZE_Y * CHUNK_SIZE_Y || z < 0 || z >= WORLD_SIZE_Z * CHUNK_SIZE_Z)
        return nullptr;

    return chunks[(x / CHUNK_SIZE_X) * WORLD_SIZE_Z + (z / CHUNK_SIZE_Z)].getVoxel(x % CHUNK_SIZE_X, y % CHUNK_SIZE_Y, z % CHUNK_SIZE_Z);
}

Voxel * World::getVoxel(glm::vec3 position) {
    return getVoxel(position.x, position.y, position.z);
}

bool World::placeVoxel(int x, int y, int z, TextureType type) {
    if (x < 0 || x >= WORLD_SIZE_X * CHUNK_SIZE_X || y < 0 || y >= WORLD_SIZE_Y * CHUNK_SIZE_Y || z < 0 || z >= WORLD_SIZE_Z * CHUNK_SIZE_Z)
        return false;

    return chunks[(x / CHUNK_SIZE_X) * WORLD_SIZE_Z + (z / CHUNK_SIZE_Z)].placeVoxel(x % CHUNK_SIZE_X, y % CHUNK_SIZE_Y, z % CHUNK_SIZE_Z, type);
}

void World::destroyVoxel(int x, int y, int z) {
    if (x < 0 || x >= WORLD_SIZE_X * CHUNK_SIZE_X || y < 0 || y >= WORLD_SIZE_Y * CHUNK_SIZE_Y || z < 0 || z >= WORLD_SIZE_Z * CHUNK_SIZE_Z)
        return;

    return chunks[(x / CHUNK_SIZE_X) * WORLD_SIZE_Z + (z / CHUNK_SIZE_Z)].destroyVoxel(x % CHUNK_SIZE_X, y % CHUNK_SIZE_Y, z % CHUNK_SIZE_Z);
}

void World::updateLoadedChunks(const Camera &camera) {
    if (cameraChunkPos == getCameraChunkCoords(camera))
        return;

    std::vector<Chunk*> newChunks;
    newChunks.reserve(4 * LOADED_CHUNK_RANGE - 1);
    cameraChunkPos = getCameraChunkCoords(camera);

    int xStart = std::max(0, cameraChunkPos.x - LOADED_CHUNK_RANGE), xEnd = std::min(WORLD_SIZE_X, cameraChunkPos.x + LOADED_CHUNK_RANGE);
    int zStart = std::max(0, cameraChunkPos.y - LOADED_CHUNK_RANGE), zEnd = std::min(WORLD_SIZE_Z, cameraChunkPos.y + LOADED_CHUNK_RANGE);

    for (int x = xStart; x < xEnd; ++x) {
        for (int z = zStart; z < zEnd; ++z) {
            if (!chunks[x * WORLD_SIZE_Z + z].initialized) {
                createChunk(x, z);
                newChunks.push_back(&(chunks[x * WORLD_SIZE_Z + z]));
            }
        }
    }

    ThreadPool::waitForAllJobs();

    for (auto newChunk : newChunks) {
        ThreadPool::QueueJob([newChunk] {newChunk->createMesh();});
    }

    ThreadPool::waitForAllJobs();

    for (auto newChunk : newChunks) {
        newChunk->loadMesh();
    }

}

glm::vec<2, int> World::getCameraChunkCoords(const Camera &camera) {
    return {camera.Position.x / CHUNK_SIZE_X, camera.Position.z / CHUNK_SIZE_Z};
}

void World::draw(const Camera &camera) {
    auto cameraChunkCoords = getCameraChunkCoords(camera);
    for (int x = std::max(0, cameraChunkCoords.x - MAX_RENDER_DISTANCE); x < std::min(WORLD_SIZE_X, cameraChunkCoords.x + MAX_RENDER_DISTANCE); x++) {
        for (int z = std::max(0, cameraChunkCoords.y - MAX_RENDER_DISTANCE); z < std::min(WORLD_SIZE_Z, cameraChunkCoords.y + MAX_RENDER_DISTANCE); ++z) {
            glm::vec<2, int> chunkCoords(x, z);
            if (length((glm::vec2)(cameraChunkCoords - chunkCoords)) <= MAX_RENDER_DISTANCE)
                chunks[x * WORLD_SIZE_Z + z].draw(camera.GetMatrices());
        }
    }
}

World::~World() {
    free(world);
    delete shader;
}
