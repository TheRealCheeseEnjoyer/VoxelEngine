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

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 16
#define CHUNK_SIZE_Z 16

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoords;
    Vertex(glm::vec3 position, glm::vec2 texCoords = {0, 0}) : position(position), texCoords(texCoords) {}
};

class Chunk {
private:
    Chunk* northChunk = nullptr;
    Chunk* southChunk = nullptr;
    Chunk* eastChunk = nullptr;
    Chunk* westChunk = nullptr;

    static int mod(int a, int b) {
        return (a % b + b) % b;
    }

    void getVolumeCoordinates(int &startX, int &startY, int &startZ, auto &meshTrackerTable, int& length, int& width, int& height) {
        length = 0, width = 0, height = 0;
        TextureType currentType = getVoxel(startX, startY, startZ)->type;

        //Ugly ahh algo but should be fine
        //maybe I'll fix it later

        for (int x = startX + 1; x < CHUNK_SIZE_X; ++x) {
            if (getVoxel(x, startY, startZ) == nullptr || getVoxel(x, startY, startZ)->type != currentType || meshTrackerTable[x][startY][startZ])
                break;

            length++;
        }

        for (int y = startY + 1; y < CHUNK_SIZE_Y; ++y) {
            bool isWholeLineOk = true;
            for (int x = startX; x <= startX + length; ++x) {
                if (getVoxel(x, y, startZ) == nullptr || getVoxel(x, y, startZ)->type != currentType || meshTrackerTable[x][y][startZ]) {
                    isWholeLineOk = false;
                    break;
                }
            }

            if (!isWholeLineOk)
                break;

            height++;
        }

        for (int z = startZ + 1; z < CHUNK_SIZE_Z; ++z) {
            bool isWholeAreOk = true;
            for (int y = startY; y <= startY + height; ++y) {
                for (int x = startX; x <= startX + length; ++x) {
                    if (getVoxel(x, y, z) == nullptr || getVoxel(x, y, z)->type != currentType || meshTrackerTable[x][y][z]) {
                        isWholeAreOk = false;
                        break;
                    }
                }

                if (!isWholeAreOk)
                    break;
            }

            if (!isWholeAreOk)
                break;

            width++;
        }

        for (int z = startZ; z <= startZ + width; ++z) {
            for (int y = startY; y <= startY + height; ++y) {
                for (int x = startX; x <= startX + length; ++x) {
                    meshTrackerTable[x][y][z] = true;
                }
            }
        }
    }

    void createMesh() {
        glm::vec3 topLeftFront, bottomRightBack;

        // Tracks which voxels have already been meshed
        std::array<std::array<std::array<bool, CHUNK_SIZE_Z>, CHUNK_SIZE_Y>, CHUNK_SIZE_X> meshTrackerTable{};
        memset(&meshTrackerTable, 0, CHUNK_SIZE_Z * CHUNK_SIZE_Y * CHUNK_SIZE_X);

        for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
            for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
                for (int x = 0; x < CHUNK_SIZE_X; ++x) {
                    Voxel* voxel = getVoxel(x, y, z);
                    if (voxel == nullptr || meshTrackerTable[x][y][z])
                        continue;

                    int length, width, height;

                    getVolumeCoordinates(x, y, z, meshTrackerTable, length, width, height);
                    bottomRightBack = {x - .5f, y - .5f, z - .5f};
                    topLeftFront = {x + length + .5f, y + height + .5f, z + width + .5f};

                    length++;
                    height++;
                    width++;

                    if (!mesh.contains(voxel->type))
                        mesh.insert({voxel->type, {}});

                    auto &vertices = mesh[voxel->type];

                    // Left Face
                    vertices.emplace_back(topLeftFront, glm::vec2(0, height));
                    vertices.emplace_back(glm::vec3(topLeftFront.x, bottomRightBack.y, topLeftFront.z), glm::vec2(0, 0));
                    vertices.emplace_back(glm::vec3(topLeftFront.x, bottomRightBack.y, bottomRightBack.z), glm::vec2(width, 0));
                    vertices.emplace_back(glm::vec3(topLeftFront.x, bottomRightBack.y, bottomRightBack.z), glm::vec2(width, 0));
                    vertices.emplace_back(glm::vec3(topLeftFront.x, topLeftFront.y, bottomRightBack.z), glm::vec2(width, height));
                    vertices.emplace_back(topLeftFront, glm::vec2(0, height));

                    // Right Face
                    vertices.emplace_back(glm::vec3(bottomRightBack.x, topLeftFront.y, bottomRightBack.z), glm::vec2(0, height));
                    vertices.emplace_back(bottomRightBack, glm::vec2(0, 0));
                    vertices.emplace_back(glm::vec3(bottomRightBack.x, bottomRightBack.y, topLeftFront.z), glm::vec2(width, 0));
                    vertices.emplace_back(glm::vec3(bottomRightBack.x, bottomRightBack.y, topLeftFront.z), glm::vec2(width, 0));
                    vertices.emplace_back(glm::vec3(bottomRightBack.x, topLeftFront.y, topLeftFront.z), glm::vec2(width, height));
                    vertices.emplace_back(glm::vec3(bottomRightBack.x, topLeftFront.y, bottomRightBack.z), glm::vec2(0, height));

                    // Top Face
                    vertices.emplace_back(topLeftFront, glm::vec2(0, width));
                    vertices.emplace_back(glm::vec3(topLeftFront.x, topLeftFront.y, bottomRightBack.z), glm::vec2(0, 0));
                    vertices.emplace_back(glm::vec3(bottomRightBack.x, topLeftFront.y, bottomRightBack.z), glm::vec2(length, 0));
                    vertices.emplace_back(glm::vec3(bottomRightBack.x, topLeftFront.y, bottomRightBack.z), glm::vec2(length, 0));
                    vertices.emplace_back(glm::vec3(bottomRightBack.x, topLeftFront.y, topLeftFront.z), glm::vec2(length, width));
                    vertices.emplace_back(topLeftFront, glm::vec2(0, width));

                    // Bottom Face
                    vertices.emplace_back(glm::vec3(bottomRightBack.x, bottomRightBack.y, topLeftFront.z), glm::vec2(0, width));
                    vertices.emplace_back(bottomRightBack, glm::vec2(0, 0));
                    vertices.emplace_back(glm::vec3(topLeftFront.x, bottomRightBack.y, bottomRightBack.z), glm::vec2(length, 0));
                    vertices.emplace_back(glm::vec3(topLeftFront.x, bottomRightBack.y, bottomRightBack.z), glm::vec2(length, 0));
                    vertices.emplace_back(glm::vec3(topLeftFront.x, bottomRightBack.y, topLeftFront.z), glm::vec2(length, width));
                    vertices.emplace_back(glm::vec3(bottomRightBack.x, bottomRightBack.y, topLeftFront.z), glm::vec2(0, width));

                    // Front Face
                    vertices.emplace_back(glm::vec3(bottomRightBack.x, topLeftFront.y, topLeftFront.z), glm::vec2(0, height));
                    vertices.emplace_back(glm::vec3(bottomRightBack.x, bottomRightBack.y, topLeftFront.z), glm::vec2(0, 0));
                    vertices.emplace_back(glm::vec3(topLeftFront.x, bottomRightBack.y, topLeftFront.z), glm::vec2(length, 0));
                    vertices.emplace_back(glm::vec3(topLeftFront.x, bottomRightBack.y, topLeftFront.z), glm::vec2(length, 0));
                    vertices.emplace_back(topLeftFront, glm::vec2(length, height));
                    vertices.emplace_back(glm::vec3(bottomRightBack.x, topLeftFront.y, topLeftFront.z), glm::vec2(0, height));

                    // Back Face
                    vertices.emplace_back(glm::vec3(topLeftFront.x, topLeftFront.y, bottomRightBack.z), glm::vec2(0, height));
                    vertices.emplace_back(glm::vec3(topLeftFront.x, bottomRightBack.y, bottomRightBack.z), glm::vec2(0, 0));
                    vertices.emplace_back(bottomRightBack, glm::vec2(length, 0));
                    vertices.emplace_back(bottomRightBack, glm::vec2(length, 0));
                    vertices.emplace_back(glm::vec3(bottomRightBack.x, topLeftFront.y, bottomRightBack.z), glm::vec2(length, height));
                    vertices.emplace_back(glm::vec3(topLeftFront.x, topLeftFront.y, bottomRightBack.z), glm::vec2(0, height));

                    x += topLeftFront.x - x;
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
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            std::advance(it, 1);
        }

        glBindVertexArray(0);
    }

public:
    glm::vec3 position;
    glm::mat4 transform = glm::mat4(1);
    unsigned int VAO = 0;
    std::vector<unsigned int> vbos;
    Shader* shader = nullptr;

    std::array<std::array<std::array<Voxel*, CHUNK_SIZE_Z>, CHUNK_SIZE_Y>, CHUNK_SIZE_X> voxels{};

    std::unordered_map<TextureType, std::vector<Vertex>> mesh;

    explicit Chunk(glm::vec2 position, Shader* shader) : position(position.x, 0, position.y), shader(shader) {
        transform = glm::translate(transform, {position.x * CHUNK_SIZE_X, 0, position.y * CHUNK_SIZE_Z});
        memset(&voxels, 0, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);

        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            for (int y = 0; y < 5; y++) {
                for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                    if (x == 5 && z == 4)
                        continue;
                    voxels[x][y][z] = new Voxel(y == 0 ? TEXTURE_BEDROCK : TEXTURE_DEFAULT);
                }
            }
        }
    }

    void init(Chunk* north, Chunk* south, Chunk* east, Chunk* west) {
        northChunk = north;
        southChunk = south;
        eastChunk = east;
        westChunk = west;

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
            return eastChunk->getVoxel(mod(x, CHUNK_SIZE_X), y, z);
        }

        if (x >= CHUNK_SIZE_X) {
            if (westChunk == nullptr)
                return nullptr;
            return westChunk->getVoxel(mod(x, CHUNK_SIZE_X), y, z);
        }

        if (z < 0) {
            if (southChunk == nullptr)
                return nullptr;
            return southChunk->getVoxel(x, y, mod(z, CHUNK_SIZE_Z));
        }

        if (z >= CHUNK_SIZE_Z) {
            if (northChunk == nullptr)
                return nullptr;
            return northChunk->getVoxel(x, y, mod(z, CHUNK_SIZE_Z));
        }

        return nullptr;
    }

    void draw(const glm::mat4 &matrices) const {
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        shader->use();
        shader->setInt("TextureUnitId", 0);
        shader->setMat4("stuff", matrices * transform);

        auto it = vbos.begin();
        for (const auto &[type, vertices]: mesh) {
            glBindBuffer(GL_ARRAY_BUFFER, *it.base());
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glBindTexture(GL_TEXTURE_2D, TextureManager::getTextureId(type));

            glDrawArrays(GL_TRIANGLES, 0, (int) vertices.size());
            it++;
        }
    }
};

#endif
