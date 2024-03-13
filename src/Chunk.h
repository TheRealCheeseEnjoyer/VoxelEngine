#ifndef VOXELENGINE_CHUNK_H
#define VOXELENGINE_CHUNK_H

#include <glm/vec2.hpp>
#include <unordered_map>
#include <array>
#include "Voxel.h"
#include "stb_image.h"

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 16
#define CHUNK_SIZE_Z 16

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
};

class Chunk {
public:
    glm::vec3 position;
    glm::mat4 transform = glm::mat4(1);
    unsigned int VAO{};
    unsigned int VBO{};
    unsigned int texture{};
    Shader* shader;
    std::array<std::array<std::array<Voxel*, CHUNK_SIZE_Z>, CHUNK_SIZE_Y>, CHUNK_SIZE_X> voxels{{{{{nullptr}}}}};
    std::vector<Vertex> mesh;

    explicit Chunk(glm::vec3 position) : position(position.x *  CHUNK_SIZE_X, position.y  * CHUNK_SIZE_Y, position.z  * CHUNK_SIZE_Z) {
        transform = glm::translate(transform, this->position);

        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            for (int y = 0; y < CHUNK_SIZE_Y; y++) {
                voxels[x][y].fill(nullptr);
                for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                    voxels[x][y][z] = new Voxel();
                }
            }
        }

        shader = Voxel::shader;

        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            for (int y = 0; y < CHUNK_SIZE_Y; y++) {
                for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                    if (getVoxel(x, y + 1, z) == nullptr) {
                        mesh.push_back({{x  - .5, y + .5, z + .5}, {1, 1}});
                        mesh.push_back({{x  + .5, y + .5, z + .5}, {0, 1}});
                        mesh.push_back({{x  - .5, y + .5, z - .5}, {1, 0}});
                        mesh.push_back({{x  - .5, y + .5, z - .5}, {1, 0}});
                        mesh.push_back({{x  + .5, y + .5, z + .5}, {0, 1}});
                        mesh.push_back({{x  + .5, y + .5, z - .5}, {0, 0}});

                    }
                    if (getVoxel(x, y - 1, z) == nullptr) {
                        mesh.push_back({{x  + .5, y - .5, z + .5}, {1, 1}});
                        mesh.push_back({{x  - .5, y - .5, z + .5}, {0, 1}});
                        mesh.push_back({{x  - .5, y - .5, z - .5}, {0, 0}});
                        mesh.push_back({{x  - .5, y - .5, z - .5}, {0, 0}});
                        mesh.push_back({{x  + .5, y - .5, z - .5}, {1, 0}});
                        mesh.push_back({{x  + .5, y - .5, z + .5}, {1, 1}});
                    }
                    if (getVoxel(x, y, z - 1) == nullptr) {
                        mesh.push_back({{x  - .5, y + .5, z - .5}, {1, 1}});
                        mesh.push_back({{x  + .5, y + .5, z - .5}, {0, 1}});
                        mesh.push_back({{x  - .5, y - .5, z - .5}, {1, 0}});
                        mesh.push_back({{x  - .5, y - .5, z - .5}, {1, 0}});
                        mesh.push_back({{x  + .5, y + .5, z - .5}, {0, 1}});
                        mesh.push_back({{x  + .5, y - .5, z - .5}, {0, 0}});
                    }
                    if (getVoxel(x, y, z + 1) == nullptr) {
                        mesh.push_back({{x  + .5, y + .5, z + .5}, {1, 1}});
                        mesh.push_back({{x  - .5, y + .5, z + .5}, {0, 1}});
                        mesh.push_back({{x  - .5, y - .5, z + .5}, {0, 0}});
                        mesh.push_back({{x  - .5, y - .5, z + .5}, {0, 0}});
                        mesh.push_back({{x  + .5, y - .5, z + .5}, {1, 0}});
                        mesh.push_back({{x  + .5, y + .5, z + .5}, {1, 1}});
                    }
                    if (getVoxel(x + 1, y, z) == nullptr)  {
                        mesh.push_back({{x  + .5, y + .5, z + .5}, {0, 1}});
                        mesh.push_back({{x  + .5, y - .5, z + .5}, {0, 0}});
                        mesh.push_back({{x  + .5, y - .5, z - .5}, {1, 0}});
                        mesh.push_back({{x  + .5, y - .5, z - .5}, {1, 0}});
                        mesh.push_back({{x  + .5, y + .5, z - .5}, {1, 1}});
                        mesh.push_back({{x  + .5, y + .5, z + .5}, {0, 1}});
                    }
                    if (getVoxel(x - 1, y, z) == nullptr) {
                        mesh.push_back({{x  - .5, y - .5, z + .5}, {1, 0}});
                        mesh.push_back({{x  - .5, y + .5, z + .5}, {1, 1}});
                        mesh.push_back({{x  - .5, y - .5, z - .5}, {0, 0}});
                        mesh.push_back({{x  - .5, y - .5, z - .5}, {0, 0}});
                        mesh.push_back({{x  - .5, y + .5, z + .5}, {1, 1}});
                        mesh.push_back({{x  - .5, y + .5, z - .5}, {0, 1}});
                    }

                    // TODO: this
                    glGenVertexArrays(1, &VAO);
                    glBindVertexArray(VAO);

                    glGenBuffers(1, &VBO);
                    glBindBuffer(GL_ARRAY_BUFFER, VBO);
                    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(mesh.size() * 5 * sizeof(float)), mesh.data(), GL_STATIC_DRAW);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
                    glEnableVertexAttribArray(0);

                    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
                    glEnableVertexAttribArray(1);

                    glActiveTexture(GL_TEXTURE0);
                    glGenTextures(1, &texture);
                    glBindTexture(GL_TEXTURE_2D, texture);
                    stbi_set_flip_vertically_on_load(true);
                    int width, height, nrChannels;
                    unsigned char* data = stbi_load("assets/image.png", &width, &height, &nrChannels, 0);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                    glGenerateMipmap(GL_TEXTURE_2D);
                    stbi_image_free(data);
                    shader->setInt("TexId", 0);

                    glBindVertexArray(0);
                }
            }
        }
    }

    Voxel* getVoxel(int x, int y, int z) {
        if (x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || z < 0 || z >= CHUNK_SIZE_Z)
            return nullptr;

        return voxels[x][y][z];
    }

    void draw(glm::mat4 viewMatrix, glm::mat4 projection) const {
        shader->use();
        shader->setMat4("stuff", projection * viewMatrix * transform);
        //shader->setInt("TexId", 0);
        glBindVertexArray(VAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 1);
        glDrawArrays(GL_TRIANGLES, 0, (int)mesh.size());
    }
};

#endif
