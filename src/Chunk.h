#ifndef VOXELENGINE_CHUNK_H
#define VOXELENGINE_CHUNK_H

#include <glm/vec2.hpp>
#include <unordered_map>
#include <array>
#include <cstring>
#include <tuple>
#include <glm/ext/matrix_transform.hpp>

#include "Voxel.h"
#include "stb_image.h"
#include "TextureManager.h"

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 16
#define CHUNK_SIZE_Z 16

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
};

class Chunk {
private:
    static int mod(int a, int b) {
        return (a%b+b)%b;
    }

    void createMesh() {
        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            for (int y = 0; y < CHUNK_SIZE_Y; y++) {
                for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                    if (getVoxel(x, y, z) == nullptr)
                        continue;

                    auto textCoords = TextureManager::getOffset(getVoxel(x, y, z)->type);

                    bool shouldDrawTopFace = getVoxel(x, y + 1, z) == nullptr;
                    bool shouldDrawBottomFace = getVoxel(x, y - 1, z) == nullptr;

                    bool shouldDrawFrontFace;
                    bool shouldDrawBackFace;
                    bool shouldDrawLeftFace;
                    bool shouldDrawRightFace;

                    if (z - 1 < 0) {
                        shouldDrawBackFace = getVoxelFromNeighborChunk(x, y, z - 1) == nullptr;
                    } else {
                        shouldDrawBackFace = getVoxel(x, y, z - 1) == nullptr;
                    }

                    if (z + 1 >= CHUNK_SIZE_Z) {
                        shouldDrawFrontFace = getVoxelFromNeighborChunk(x, y, z + 1) == nullptr;
                    } else {
                        shouldDrawFrontFace = getVoxel(x, y, z + 1) == nullptr;
                    }

                    if (x + 1 >= CHUNK_SIZE_X) {
                        shouldDrawLeftFace = getVoxelFromNeighborChunk(x + 1, y, z) == nullptr;
                    } else {
                        shouldDrawLeftFace = getVoxel(x + 1, y, z) == nullptr;
                    }

                    if (x - 1 < 0) {
                        shouldDrawRightFace = getVoxelFromNeighborChunk(x - 1, y, z) == nullptr;
                    } else {
                        shouldDrawRightFace = getVoxel(x - 1, y, z) == nullptr;
                    }


                    if (shouldDrawTopFace) {
                        mesh.push_back({{x - .5, y + .5, z + .5}, {std::get<1>(textCoords), 1}});
                        mesh.push_back({{x + .5, y + .5, z + .5}, {std::get<0>(textCoords), 1}});
                        mesh.push_back({{x - .5, y + .5, z - .5}, {std::get<1>(textCoords), 0}});
                        mesh.push_back({{x - .5, y + .5, z - .5}, {std::get<1>(textCoords), 0}});
                        mesh.push_back({{x + .5, y + .5, z + .5}, {std::get<0>(textCoords), 1}});
                        mesh.push_back({{x + .5, y + .5, z - .5}, {std::get<0>(textCoords), 0}});

                    }

                    if (shouldDrawBottomFace) {
                        mesh.push_back({{x + .5, y - .5, z + .5}, {std::get<1>(textCoords), 1}});
                        mesh.push_back({{x - .5, y - .5, z + .5}, {std::get<0>(textCoords), 1}});
                        mesh.push_back({{x - .5, y - .5, z - .5}, {std::get<0>(textCoords), 0}});
                        mesh.push_back({{x - .5, y - .5, z - .5}, {std::get<0>(textCoords), 0}});
                        mesh.push_back({{x + .5, y - .5, z - .5}, {std::get<1>(textCoords), 0}});
                        mesh.push_back({{x + .5, y - .5, z + .5}, {std::get<1>(textCoords), 1}});
                    }

                    if (shouldDrawBackFace) {
                        mesh.push_back({{x - .5, y + .5, z - .5}, {std::get<1>(textCoords), 1}});
                        mesh.push_back({{x + .5, y + .5, z - .5}, {std::get<0>(textCoords), 1}});
                        mesh.push_back({{x - .5, y - .5, z - .5}, {std::get<1>(textCoords), 0}});
                        mesh.push_back({{x - .5, y - .5, z - .5}, {std::get<1>(textCoords), 0}});
                        mesh.push_back({{x + .5, y + .5, z - .5}, {std::get<0>(textCoords), 1}});
                        mesh.push_back({{x + .5, y - .5, z - .5}, {std::get<0>(textCoords), 0}});
                    }
                    if (shouldDrawFrontFace) {
                        mesh.push_back({{x + .5, y + .5, z + .5}, {std::get<1>(textCoords), 1}});
                        mesh.push_back({{x - .5, y + .5, z + .5}, {std::get<0>(textCoords), 1}});
                        mesh.push_back({{x - .5, y - .5, z + .5}, {std::get<0>(textCoords), 0}});
                        mesh.push_back({{x - .5, y - .5, z + .5}, {std::get<0>(textCoords), 0}});
                        mesh.push_back({{x + .5, y - .5, z + .5}, {std::get<1>(textCoords), 0}});
                        mesh.push_back({{x + .5, y + .5, z + .5}, {std::get<1>(textCoords), 1}});
                    }
                    if (shouldDrawLeftFace) {
                        mesh.push_back({{x + .5, y + .5, z + .5}, {std::get<0>(textCoords), 1}});
                        mesh.push_back({{x + .5, y - .5, z + .5}, {std::get<0>(textCoords), 0}});
                        mesh.push_back({{x + .5, y - .5, z - .5}, {std::get<1>(textCoords), 0}});
                        mesh.push_back({{x + .5, y - .5, z - .5}, {std::get<1>(textCoords), 0}});
                        mesh.push_back({{x + .5, y + .5, z - .5}, {std::get<1>(textCoords), 1}});
                        mesh.push_back({{x + .5, y + .5, z + .5}, {std::get<0>(textCoords), 1}});
                    }
                    if (shouldDrawRightFace) {
                        mesh.push_back({{x - .5, y - .5, z + .5}, {std::get<1>(textCoords), 0}});
                        mesh.push_back({{x - .5, y + .5, z + .5}, {std::get<1>(textCoords), 1}});
                        mesh.push_back({{x - .5, y - .5, z - .5}, {std::get<0>(textCoords), 0}});
                        mesh.push_back({{x - .5, y - .5, z - .5}, {std::get<0>(textCoords), 0}});
                        mesh.push_back({{x - .5, y + .5, z + .5}, {std::get<1>(textCoords), 1}});
                        mesh.push_back({{x - .5, y + .5, z - .5}, {std::get<0>(textCoords), 1}});
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

        if (VBO == 0) {
            glGenBuffers(1, &VBO);
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (mesh.size() * 5 * sizeof(float)), mesh.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    Chunk* northChunk = nullptr;
    Chunk* southChunk = nullptr;
    Chunk* eastChunk = nullptr;
    Chunk* westChunk = nullptr;

public:
    glm::vec3 position;
    glm::mat4 transform = glm::mat4(1);
    unsigned int VAO = 0, VBO = 0;
    Shader* shader = nullptr;
    std::array<std::array<std::array<Voxel*, CHUNK_SIZE_Z>, CHUNK_SIZE_Y>, CHUNK_SIZE_X> voxels{};
    std::vector<Vertex> mesh;

    explicit Chunk(glm::vec2 position) : position(position.x * CHUNK_SIZE_X, 0, position.y * CHUNK_SIZE_Z) {
        transform = glm::translate(transform, this->position);
        memset(&voxels, 0, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);

        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            for (int y = 0; y < 5; y++) {
                for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                    voxels[x][y][z] = new Voxel(y == 0 ? TEXTURE_BEDROCK : TEXTURE_DEFAULT);
                }
            }
        }

        shader = Voxel::shader;
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

    void draw(const glm::mat4& matrices) const {
        shader->use();
        shader->setMat4("stuff", matrices * transform);
        shader->setInt("TextureUnitId", 0);
        glBindVertexArray(VAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureManager::getTextureId(TEXTURE_DEFAULT));
        glDrawArrays(GL_TRIANGLES, 0, (int) mesh.size());
    }
};

#endif
