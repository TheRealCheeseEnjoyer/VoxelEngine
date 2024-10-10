#ifndef PLAYER_H
#define PLAYER_H

#define MAX_RANGE 2

#include "Camera.h"
#include "World.h"

class Player {
    Chunk* currentChunk = nullptr;
    Camera camera;
public:
    Player(glm::vec3 position = {0, 0, 0}) : camera(position) {
        //currentChunk = getChunk(position.x / CHUNK_SIZE_X, position.z / CHUNK_SIZE_Z);
    }

    Camera* getCamera() {
        return &camera;
    }

    void Update(const float deltaTime) {
        auto mouseDelta = InputManager::getMouseDelta();
        camera.ProcessMouseMovement(mouseDelta.x, mouseDelta.y);

        auto direction = InputManager::getMovementInput();
        camera.ProcessKeyboard(direction, deltaTime);

        glm::vec3 ray = camera.Front * glm::vec3(MAX_RANGE);

        glm::vec3 currentBlockCoords = {round(camera.Position.x), round(camera.Position.y), round(camera.Position.z)};
        glm::vec3 currentBlock;
        float minDistance;
        /*for (int x = 0; x < MAX_RANGE; x++) {
            for (int y = 0; y < MAX_RANGE; y++) {
                for (int z = 0; z < MAX_RANGE; z++) {
                    glm::vec<3, int> blockCoords = currentBlockCoords;
                    blockCoords.x += (ray.x > 0) ? x : -x;
                    blockCoords.y += (ray.y > 0) ? y : -y;
                    blockCoords.z += (ray.z > 0) ? z : -z;

                    if (currentChunk->getVoxel(blockCoords.x, blockCoords.y, blockCoords.z) == nullptr) {
                        continue;
                    }

                    float boxMinX = blockCoords.x - .5f, boxMinY = blockCoords.y - .5f, boxMinZ = blockCoords.z - .5f;
                    float boxMaxX = blockCoords.x + .5f, boxMaxY = blockCoords.y + .5f, boxMaxZ = blockCoords.z + .5f;



                }
            }
        }*/
    }
};

#endif
