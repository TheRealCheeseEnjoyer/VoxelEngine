#ifndef VOXELENGINE_CHUNK_H
#define VOXELENGINE_CHUNK_H

#include <array>
#include <unordered_map>
#include <glm/vec2.hpp>
#include "Shader.h"

#include "InputManager.h"
#include "PerlinNoise.h"
#include "TextureManager.h"
#include "Voxel.h"

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 16
#define CHUNK_SIZE_Z 16

typedef std::array<std::array<std::array<char, CHUNK_SIZE_Z>, CHUNK_SIZE_Y>, CHUNK_SIZE_X> MeshedFacesTable;

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoords;

    Vertex(glm::vec3 position, glm::vec2 texCoords) : position(position), texCoords(texCoords) {}
};

enum FaceOrientation {
    FACE_TOP = 0b1,
    FACE_BOTTOM = 0b10,
    FACE_LEFT = 0b100,
    FACE_RIGHT = 0b1000,
    FACE_FRONT = 0b10000,
    FACE_BACK = 0b100000
};

class Chunk {
public:
    bool initialized = false;
    glm::vec3 position;
    glm::mat4 transform = glm::mat4(1);
    unsigned int VAO = 0;
    std::unordered_map<TextureType, unsigned int> vbos;
    Shader* shader = nullptr;
    GLenum polygonMode = GL_FILL;

    Chunk* northChunk = nullptr;
    Chunk* southChunk = nullptr;
    Chunk* eastChunk = nullptr;
    Chunk* westChunk = nullptr;

    Voxel* voxels = nullptr;
    std::unordered_map<TextureType, std::vector<Vertex>> mesh;

    void createMesh(int meshMask = -1);

    void loadMesh(int meshMask = -1);

    static int mod(int a, int b);

    void getSurfaceBounds(glm::vec<3, int> start, glm::vec2 &dimensions, FaceOrientation orientation, MeshedFacesTable &meshedFaces) const;

    Chunk(glm::vec2 position, Shader* shader, const siv::PerlinNoise &noise, Chunk* north, Chunk* south,
                   Chunk* east, Chunk* west, Voxel* voxelArray);

    Voxel* getVoxel(int x, int y, int z) const;

    void neighborBlockDestroyed(int x, int y, int z);

    bool placeVoxel(int x, int y, int z, TextureType type);

    void destroyVoxel(int x, int y, int z);

    glm::vec2 chunkToWorldCoordinate(int x, int z) const;

    void draw(const glm::mat4 &matrices);
};

#endif
