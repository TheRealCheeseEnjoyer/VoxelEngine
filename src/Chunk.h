#ifndef VOXELENGINE_CHUNK_H
#define VOXELENGINE_CHUNK_H

#include <glm/vec2.hpp>
#include <unordered_map>
#include <array>
#include <cstring>
#include <tuple>
#include <glm/ext/matrix_transform.hpp>
#include <forward_list>

#include "Voxel.h"
#include "stb_image.h"
#include "TextureManager.h"
#include "PerlinNoise.h"

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 16
#define CHUNK_SIZE_Z 16

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
    void createMesh() {
        // Tracks which voxels have already been meshed
        std::array<std::array<std::array<char, CHUNK_SIZE_Z>, CHUNK_SIZE_Y>, CHUNK_SIZE_X> meshTrackerTable{};
        memset(&meshTrackerTable, 0, CHUNK_SIZE_Z * CHUNK_SIZE_Y * CHUNK_SIZE_X);

        for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
            for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
                for (int x = 0; x < CHUNK_SIZE_X; ++x) {
                    Voxel* voxel = getVoxel(x, y, z);
                    if (voxel == nullptr)
                        continue;

                    if (!mesh.contains(voxel->type))
                        mesh.insert({voxel->type, {}});

                    auto &vertices = mesh[voxel->type];

                    glm::vec2 dimensions;
                    if (getVoxel(x, y + 1, z) == nullptr && (meshTrackerTable[x][y][z] & FACE_TOP) != FACE_TOP) {
                        getSurfaceCoordinates({x, y, z}, dimensions, FACE_TOP, meshTrackerTable);
                        vertices.emplace_back(glm::vec3(x + dimensions.x + .5f, y + .5f, z - .5f), glm::vec2(0, 0));
                        vertices.emplace_back(glm::vec3(x - .5f, y + .5f, z - .5f), glm::vec2(dimensions.x + 1, 0));
                        vertices.emplace_back(glm::vec3(x - .5f, y + .5f, z + dimensions.y + .5f),
                                              glm::vec2(dimensions.x + 1, dimensions.y + 1));
                        vertices.emplace_back(glm::vec3(x - .5f, y + .5f, z + dimensions.y + .5f),
                                              glm::vec2(dimensions.x + 1, dimensions.y + 1));
                        vertices.emplace_back(glm::vec3(x + dimensions.x + .5f, y + .5f, z + dimensions.y + .5f),
                                              glm::vec2(0, dimensions.y + 1));
                        vertices.emplace_back(glm::vec3(x + dimensions.x + .5f, y + .5f, z - .5f), glm::vec2(0, 0));
                    }
                    if (getVoxel(x, y - 1, z) == nullptr && (meshTrackerTable[x][y][z] & FACE_BOTTOM) != FACE_BOTTOM) {
                        getSurfaceCoordinates({x, y, z}, dimensions, FACE_BOTTOM, meshTrackerTable);
                        vertices.emplace_back(glm::vec3(x - .5f, y - .5f, z + dimensions.y + .5f),
                                              glm::vec2(-dimensions.x - 1, dimensions.y + 1));
                        vertices.emplace_back(glm::vec3(x - .5f, y - .5f, z - .5f), glm::vec2(-dimensions.x - 1, 0));
                        vertices.emplace_back(glm::vec3(x + dimensions.x + .5f, y - .5f, z - .5f), glm::vec2(0, 0));
                        vertices.emplace_back(glm::vec3(x + dimensions.x + .5f, y - .5f, z - .5f), glm::vec2(0, 0));
                        vertices.emplace_back(glm::vec3(x + dimensions.x + .5f, y - .5f, z + dimensions.y + .5f),
                                              glm::vec2(0, dimensions.y + 1));
                        vertices.emplace_back(glm::vec3(x - .5f, y - .5f, z + dimensions.y + .5f),
                                              glm::vec2(-dimensions.x - 1, dimensions.y + 1));
                    }
                    if ((x + 1 >= CHUNK_SIZE_X ? getVoxelFromNeighborChunk(x + 1, y, z) : getVoxel(x + 1, y, z)) ==
                        nullptr && (meshTrackerTable[x][y][z] & FACE_LEFT) != FACE_LEFT) {
                        getSurfaceCoordinates({x, y, z}, dimensions, FACE_LEFT, meshTrackerTable);
                        vertices.emplace_back(glm::vec3(x + .5f, y - .5f, z + dimensions.x + .5f), glm::vec2(0, 0));
                        vertices.emplace_back(glm::vec3(x + .5f, y - .5f, z - .5f), glm::vec2(dimensions.x + 1, 0));
                        vertices.emplace_back(glm::vec3(x + .5f, y + dimensions.y + .5f, z - .5f),
                                              glm::vec2(dimensions.x + 1, dimensions.y + 1));
                        vertices.emplace_back(glm::vec3(x + .5f, y + dimensions.y + .5f, z - .5f),
                                              glm::vec2(dimensions.x + 1, dimensions.y + 1));
                        vertices.emplace_back(glm::vec3(x + .5f, y + dimensions.y + .5f, z + dimensions.x + .5f),
                                              glm::vec2(0, dimensions.y + 1));
                        vertices.emplace_back(glm::vec3(x + .5f, y - .5f, z + dimensions.x + .5f), glm::vec2(0, 0));
                    }
                    if ((x - 1 < 0 ? getVoxelFromNeighborChunk(x - 1, y, z) : getVoxel(x - 1, y, z)) == nullptr &&
                        (meshTrackerTable[x][y][z] & FACE_RIGHT) != FACE_RIGHT) {
                        getSurfaceCoordinates({x, y, z}, dimensions, FACE_RIGHT, meshTrackerTable);
                        vertices.emplace_back(glm::vec3(x - .5f, y + dimensions.y + .5f, z - .5f),
                                              glm::vec2(-dimensions.x - 1, dimensions.y + 1));
                        vertices.emplace_back(glm::vec3(x - .5f, y - .5f, z - .5f), glm::vec2(-dimensions.x - 1, 0));
                        vertices.emplace_back(glm::vec3(x - .5f, y - .5f, z + dimensions.x + .5f), glm::vec2(0, 0));
                        vertices.emplace_back(glm::vec3(x - .5f, y - .5f, z + dimensions.x + .5f), glm::vec2(0, 0));
                        vertices.emplace_back(glm::vec3(x - .5f, y + dimensions.y + .5f, z + dimensions.x + .5f),
                                              glm::vec2(0, dimensions.y + 1));
                        vertices.emplace_back(glm::vec3(x - .5f, y + dimensions.y + .5f, z - .5f),
                                              glm::vec2(-dimensions.x - 1, dimensions.y + 1));
                    }
                    if ((z + 1 >= CHUNK_SIZE_Z ? getVoxelFromNeighborChunk(x, y, z + 1) : getVoxel(x, y, z + 1)) ==
                        nullptr && (meshTrackerTable[x][y][z] & FACE_FRONT) != FACE_FRONT) {
                        getSurfaceCoordinates({x, y, z}, dimensions, FACE_FRONT, meshTrackerTable);
                        vertices.emplace_back(glm::vec3(x - .5f, y - .5f, z + .5f), glm::vec2(0, 0));
                        vertices.emplace_back(glm::vec3(x + dimensions.x + .5f, y - .5f, z + .5f),
                                              glm::vec2(dimensions.x + 1, 0));
                        vertices.emplace_back(glm::vec3(x + dimensions.x + .5f, y + dimensions.y + .5f, z + .5f),
                                              glm::vec2(dimensions.x + 1, dimensions.y + 1));
                        vertices.emplace_back(glm::vec3(x + dimensions.x + .5f, y + dimensions.y + .5f, z + .5f),
                                              glm::vec2(dimensions.x + 1, dimensions.y + 1));
                        vertices.emplace_back(glm::vec3(x - .5f, y + dimensions.y + .5f, z + .5f),
                                              glm::vec2(0, dimensions.y + 1));
                        vertices.emplace_back(glm::vec3(x - .5f, y - .5f, z + .5f), glm::vec2(0, 0));
                    }
                    if ((z - 1 < 0 ? getVoxelFromNeighborChunk(x, y, z - 1) : getVoxel(x, y, z - 1)) == nullptr &&
                        (meshTrackerTable[x][y][z] & FACE_BACK) != FACE_BACK) {
                        getSurfaceCoordinates({x, y, z}, dimensions, FACE_BACK, meshTrackerTable);
                        vertices.emplace_back(glm::vec3(x + dimensions.x + .5f, y + dimensions.y + .5f, z - .5f),
                                              glm::vec2(-dimensions.x - 1, dimensions.y + 1));
                        vertices.emplace_back(glm::vec3(x + dimensions.x + .5f, y - .5f, z - .5f),
                                              glm::vec2(-dimensions.x - 1, 0));
                        vertices.emplace_back(glm::vec3(x - .5f, y - .5f, z - .5f), glm::vec2(0, 0));
                        vertices.emplace_back(glm::vec3(x - .5f, y - .5f, z - .5f), glm::vec2(0, 0));
                        vertices.emplace_back(glm::vec3(x - .5f, y + dimensions.y + .5f, z - .5f),
                                              glm::vec2(0, dimensions.y + 1));
                        vertices.emplace_back(glm::vec3(x + dimensions.x + .5f, y + dimensions.y + .5f, z - .5f),
                                              glm::vec2(-dimensions.x - 1, dimensions.y + 1));
                    }
                }
            }
        }
    }

    void loadMesh() {
        if (VAO == 0) {
            glGenVertexArrays(1, &VAO);
        }

        glBindVertexArray(VAO);

        if (vbos.size() < mesh.size()) {
            int n = mesh.size() - vbos.size();
            vbos.resize(mesh.size());
            glGenBuffers(n, (vbos.end() - n).base());
        }

        auto it = mesh.begin();
        for (unsigned int vbo: vbos) {
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (it->second.size() * sizeof(Vertex)), it->second.data(),
                         GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            std::advance(it, 1);
        }

        glBindVertexArray(0);
    }

private:
    Chunk** northChunk = nullptr;
    Chunk** southChunk = nullptr;
    Chunk** eastChunk = nullptr;
    Chunk** westChunk = nullptr;

    static int mod(int a, int b) {
        return (a % b + b) % b;
    }

    void getSurfaceCoordinates(glm::vec<3, int, glm::defaultp> start, glm::vec2 &dimensions, FaceOrientation orientation,
                          auto &meshTrackerTable) {
        int length = 0, width = 0;
        int startLengthDimension, startWidthDimension;
        int lengthLimit, widthLimit;
        int* lengthDimension;
        int* widthDimension;
        int lengthDiff, widthDiff;
        int neighborDirection;
        glm::vec<3, int*, glm::defaultp> neighbor{};
        TextureType currentType = getVoxel(start.x, start.y, start.z)->type;


        switch (orientation) {
            case FACE_TOP:
                lengthDimension = &start.x;
                widthDimension = &start.z;
                startLengthDimension = start.x;
                startWidthDimension = start.z;
                lengthLimit = CHUNK_SIZE_X;
                widthLimit = CHUNK_SIZE_Z;
                neighborDirection = start.y + 1;
                neighbor = {lengthDimension, &neighborDirection, widthDimension};
                break;
            case FACE_BOTTOM:
                lengthDimension = &start.x;
                widthDimension = &start.z;
                startLengthDimension = start.x;
                startWidthDimension = start.z;
                lengthLimit = CHUNK_SIZE_X;
                widthLimit = CHUNK_SIZE_Z;
                neighborDirection = start.y - 1;
                neighbor = {lengthDimension, &neighborDirection, widthDimension};
                break;
            case FACE_LEFT:
                lengthDimension = &start.z;
                widthDimension = &start.y;
                startLengthDimension = start.z;
                startWidthDimension = start.y;
                lengthLimit = CHUNK_SIZE_Z;
                widthLimit = CHUNK_SIZE_Y;
                neighborDirection = start.x + 1;
                neighbor = {&neighborDirection, widthDimension, lengthDimension};
                break;
            case FACE_RIGHT:
                lengthDimension = &start.z;
                widthDimension = &start.y;
                startLengthDimension = start.z;
                startWidthDimension = start.y;
                lengthLimit = CHUNK_SIZE_Z;
                widthLimit = CHUNK_SIZE_Y;
                neighborDirection = start.x - 1;
                neighbor = {&neighborDirection, widthDimension, lengthDimension};
                break;
            case FACE_FRONT:
                lengthDimension = &start.x;
                widthDimension = &start.y;
                startLengthDimension = start.x;
                startWidthDimension = start.y;
                lengthLimit = CHUNK_SIZE_X;
                widthLimit = CHUNK_SIZE_Y;
                neighborDirection = start.z + 1;
                neighbor = {lengthDimension, widthDimension, &neighborDirection};
                break;
            case FACE_BACK:
                lengthDimension = &start.x;
                widthDimension = &start.y;
                startLengthDimension = start.x;
                startWidthDimension = start.y;
                lengthLimit = CHUNK_SIZE_X;
                widthLimit = CHUNK_SIZE_Y;
                neighborDirection = start.z - 1;
                neighbor = {lengthDimension, widthDimension, &neighborDirection};
                break;
        }

        for (*lengthDimension += 1; *lengthDimension < lengthLimit; (*lengthDimension)++) {
            if ((meshTrackerTable[start.x][start.y][start.z] & orientation) == orientation ||
                getVoxel(start.x, start.y, start.z) == nullptr ||
                getVoxel(start.x, start.y, start.z)->type != currentType ||
                getVoxel(*neighbor.x, *neighbor.y, *neighbor.z) != nullptr) {
                break;
            }

            length++;
        }

        for (*widthDimension += 1; *widthDimension < widthLimit; (*widthDimension)++) {
            bool isWholeLineOk = true;
            for (*lengthDimension = startLengthDimension;
                 *lengthDimension <= startLengthDimension + length; (*lengthDimension)++) {
                if ((meshTrackerTable[start.x][start.y][start.z] & orientation) == orientation ||
                    getVoxel(start.x, start.y, start.z) == nullptr ||
                    getVoxel(start.x, start.y, start.z)->type != currentType ||
                    getVoxel(*neighbor.x, *neighbor.y, *neighbor.z) != nullptr) {
                    isWholeLineOk = false;
                    break;
                }
            }

            if (!isWholeLineOk)
                break;

            width++;
        }

        for (*widthDimension = startWidthDimension;
             *widthDimension <= startWidthDimension + width; (*widthDimension)++) {
            for (*lengthDimension = startLengthDimension;
                 *lengthDimension <= startLengthDimension + length; (*lengthDimension)++) {
                meshTrackerTable[start.x][start.y][start.z] ^= orientation;
            }
        }

        dimensions = {length, width};
    }

public:
    glm::vec3 position;
    glm::mat4 transform = glm::mat4(1);
    unsigned int VAO = 0;
    std::vector<unsigned int> vbos;
    Shader* shader = nullptr;
    GLenum polygonMode = GL_FILL;

    std::array<std::array<std::array<Voxel*, CHUNK_SIZE_Z>, CHUNK_SIZE_Y>, CHUNK_SIZE_X> voxels{};

    std::unordered_map<TextureType, std::vector<Vertex>> mesh;

    explicit Chunk(glm::vec2 position, Shader* shader, const siv::PerlinNoise &noise, Chunk** north, Chunk** south,
                   Chunk** east, Chunk** west) : northChunk(north), southChunk(south), eastChunk(east),
                                                 westChunk(west), position(position.x, 0, position.y), shader(shader) {
        transform = glm::translate(transform, {position.x * CHUNK_SIZE_X, 0, position.y * CHUNK_SIZE_Z});
        memset(&voxels, 0, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);
        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                float frequency = .1f;

                auto blockPos = chunkToWorldCoordinate(x, z);

                int value =
                        noise.noise2D_01(blockPos.x * frequency, blockPos.y * frequency)
                        + .5f * noise.noise2D_01(blockPos.x * frequency, blockPos.y * frequency)
                        + .25f * noise.noise2D_01(blockPos.x * frequency, blockPos.y * frequency) * CHUNK_SIZE_Y;
                for (int y = 0; y < std::fmax(1, std::fmin(value, CHUNK_SIZE_Y)); ++y) {
                    voxels[x][y][z] = new Voxel(y == 0 ? TEXTURE_BEDROCK : TEXTURE_DEFAULT);
                }
            }
        }
    }

    void init() {
        createMesh();
        loadMesh();
    }

    Voxel* getVoxel(int x, int y, int z) {
        if (x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || z < 0 || z >= CHUNK_SIZE_Z)
            return nullptr;

        return voxels[x][y][z];
    }

    Voxel* getVoxelFromNeighborChunk(int x, int y, int z) {
        if (x < 0) {
            if (eastChunk == nullptr)
                return nullptr;
            return (*eastChunk)->getVoxel(mod(x, CHUNK_SIZE_X), y, z);
        }

        if (x >= CHUNK_SIZE_X) {
            if (westChunk == nullptr)
                return nullptr;
            return (*westChunk)->getVoxel(mod(x, CHUNK_SIZE_X), y, z);
        }

        if (z < 0) {
            if (southChunk == nullptr)
                return nullptr;
            return (*southChunk)->getVoxel(x, y, mod(z, CHUNK_SIZE_Z));
        }

        if (z >= CHUNK_SIZE_Z) {
            if (northChunk == nullptr)
                return nullptr;
            return (*northChunk)->getVoxel(x, y, mod(z, CHUNK_SIZE_Z));
        }

        return nullptr;
    }

    glm::vec2 chunkToWorldCoordinate(int x, int z) const {
        return {CHUNK_SIZE_X * position.x + x, CHUNK_SIZE_Z * position.z + z};
    }

    void draw(const glm::mat4 &matrices) {
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        shader->use();
        shader->setInt("TextureUnitId", 0);
        shader->setMat4("stuff", matrices * transform);

        auto it = vbos.begin();
        if (InputManager::getMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
            if (polygonMode == GL_FILL) {
                polygonMode = GL_LINE;
            } else {
                polygonMode = GL_FILL;
            }
        }
        for (const auto &[type, vertices]: mesh) {
            glBindBuffer(GL_ARRAY_BUFFER, *it.base());
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
            glBindTexture(GL_TEXTURE_2D, TextureManager::getTextureId(type));

            glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

            glDrawArrays(GL_TRIANGLES, 0, (int) vertices.size());
            it++;
        }
    }
};

#endif
