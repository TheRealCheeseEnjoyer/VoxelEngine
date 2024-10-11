#ifndef PLAYER_H
#define PLAYER_H

#define MAX_RANGE 4

#include "Camera.h"
#include "World.h"
#include <math.h>

class Player {
    Camera camera;

    static bool rayIntersectsBlock(glm::vec3 origin, glm::vec3 direction, glm::vec3 aabb_min, glm::vec3 aabb_max, float& distance) {
        float tmin = -INFINITY, tmax = INFINITY;

        float tx1 = (aabb_min.x - origin.x) / direction.x;
        float tx2 = (aabb_max.x - origin.x) / direction.x;

        tmin = fmax(tmin, fmin(tx1, tx2));
        tmax = fmin(tmax, fmax(tx1, tx2));

        float ty1 = (aabb_min.y - origin.y) / direction.y;
        float ty2 = (aabb_max.y - origin.y) / direction.y;

        tmin = fmax(tmin, fmin(ty1, ty2));
        tmax = fmin(tmax, fmax(ty1, ty2));

        float tz1 = (aabb_min.z - origin.z) / direction.z;
        float tz2 = (aabb_max.z - origin.z) / direction.z;

        tmin = fmax(tmin, fmin(tz1, tz2));
        tmax = fmin(tmax, fmax(tz1, tz2)),

    	distance = tmin;
        return tmin <= tmax && tmax > 0;
    }

public:
    explicit Player(glm::vec3 position = {0, 0, 0}) : camera(position) {}

    Camera* getCamera() { return &camera; }

    void Update(const float deltaTime) {
        auto mouseDelta = InputManager::getMouseDelta();
        camera.ProcessMouseMovement(mouseDelta.x, mouseDelta.y);

        auto direction = InputManager::getMovementInput();
        camera.ProcessKeyboard(direction, deltaTime);

        if (!InputManager::getMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
            return;

        glm::vec3 currentBlockCoords = {round(camera.Position.x), round(camera.Position.y), round(camera.Position.z)};
        printf("%f, %f, %f\n", currentBlockCoords.x, currentBlockCoords.y, currentBlockCoords.z);

        glm::vec3 pickedBlock = {-1, -1, -1};
        glm::vec3 currentBlock;
        float minDistance = INFINITY;
        for (int x = -MAX_RANGE; x <= MAX_RANGE; x++) {
            for (int y = -MAX_RANGE; y <= MAX_RANGE; y++) {
                for (int z = -MAX_RANGE; z <= MAX_RANGE; z++) {
                    glm::vec3 blockCoords = {currentBlockCoords.x + x, currentBlockCoords.y + y, currentBlockCoords.z + z};

                    if (World::instance->getVoxel(blockCoords.x, blockCoords.y, blockCoords.z) == nullptr || World::instance->getVoxel(blockCoords.x, blockCoords.y, blockCoords.z)->type == TEXTURE_NONE)
                        continue;

                    float distance;
                    float boxMinX = blockCoords.x - .5f, boxMinY = blockCoords.y - .5f, boxMinZ = blockCoords.z - .5f;
                    float boxMaxX = blockCoords.x + .5f, boxMaxY = blockCoords.y + .5f, boxMaxZ = blockCoords.z + .5f;

                    if (rayIntersectsBlock(
                        camera.Position,
                        camera.Front,
                        {boxMinX, boxMinY, boxMinZ},
                        {boxMaxX, boxMaxY, boxMaxZ},
                        distance)) {


                        if (distance > MAX_RANGE) {
	                        continue;
                        }
                        if (distance < minDistance) {
                            minDistance = distance;
                            pickedBlock = blockCoords;
                        }
                    }
                }
            }
        }

        World::instance->destroyVoxel(pickedBlock.x, pickedBlock.y, pickedBlock.z);

        printf("Looking at block: %f, %f, %f\n", pickedBlock.x, pickedBlock.y, pickedBlock.z);
    }
};

#endif
