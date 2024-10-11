#ifndef PLAYER_H
#define PLAYER_H

#define MAX_RANGE 4

#include "Camera.h"
#include "World.h"
#include <math.h>

class Player {
    Camera camera;
    glm::vec3 blockLookedAt = {-1, -1, -1};
    FaceOrientation faceLookedAt;

    static bool rayIntersectsBlock(glm::vec3 origin, glm::vec3 direction, glm::vec3 aabb_min, glm::vec3 aabb_max, float& distance, FaceOrientation& face) {
        float tmin = -INFINITY, tmax = INFINITY;

        FaceOrientation yFace = (FaceOrientation)-1, xFace = (FaceOrientation)-1, zFace = (FaceOrientation)-1;
        if (origin.y > aabb_max.y)
            yFace = FACE_TOP;
        else if (origin.y < aabb_min.y)
            yFace = FACE_BOTTOM;

        if (origin.x > aabb_max.x)
            xFace = FACE_LEFT;
        else if (origin.x < aabb_min.x)
            xFace = FACE_RIGHT;

        if (origin.z > aabb_max.z)
            zFace = FACE_FRONT;
        else if (origin.z < aabb_min.z)
            zFace = FACE_BACK;

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
        glm::vec3 ray = origin + direction * distance;
        if (ray.y > aabb_min.y && ray.y < aabb_max.y && ray.z > aabb_min.z && ray.z < aabb_max.z) {
            face = xFace;
        } else if (ray.y > aabb_min.y && ray.y < aabb_max.y && ray.x > aabb_min.x && ray.x < aabb_max.x) {
            face = zFace;
        } else if (ray.x > aabb_min.x && ray.x < aabb_max.x && ray.z > aabb_min.z && ray.z < aabb_max.z) {
            face = yFace;
        }

        return tmin <= tmax && tmax > 0;
    }

    void getBlockLookedAt() {
        blockLookedAt = {-1, -1, -1};

        glm::vec3 currentBlockCoords = {round(camera.Position.x), round(camera.Position.y), round(camera.Position.z)};
        float minDistance = INFINITY;
        for (int x = -MAX_RANGE; x <= MAX_RANGE; x++) {
            for (int y = -MAX_RANGE; y <= MAX_RANGE; y++) {
                for (int z = -MAX_RANGE; z <= MAX_RANGE; z++) {
                    glm::vec3 blockCoords = {currentBlockCoords.x + x, currentBlockCoords.y + y, currentBlockCoords.z + z};

                    if (World::instance->getVoxel(blockCoords.x, blockCoords.y, blockCoords.z) == nullptr || World::instance->getVoxel(blockCoords.x, blockCoords.y, blockCoords.z)->type == TEXTURE_NONE)
                        continue;

                    float distance;
                    FaceOrientation face;
                    float boxMinX = blockCoords.x - .5f, boxMinY = blockCoords.y - .5f, boxMinZ = blockCoords.z - .5f;
                    float boxMaxX = blockCoords.x + .5f, boxMaxY = blockCoords.y + .5f, boxMaxZ = blockCoords.z + .5f;

                    if (rayIntersectsBlock(
                        camera.Position,
                        camera.Front,
                        {boxMinX, boxMinY, boxMinZ},
                        {boxMaxX, boxMaxY, boxMaxZ},
                        distance,
                        face)) {


                        if (distance > MAX_RANGE) {
                            continue;
                        }
                        if (distance < minDistance) {
                            faceLookedAt = face;
                            minDistance = distance;
                            blockLookedAt = blockCoords;
                        }}
                }
            }
        }
    }

public:
    explicit Player(glm::vec3 position = {0, 0, 0}) : camera(position) {}

    Camera* getCamera() { return &camera; }

    void Update(const float deltaTime) {
        auto mouseDelta = InputManager::getMouseDelta();
        camera.ProcessMouseMovement(mouseDelta.x, mouseDelta.y);

        auto direction = InputManager::getMovementInput();
        camera.ProcessKeyboard(direction, deltaTime);

        getBlockLookedAt();

        if (InputManager::getMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) {
            glm::vec3 newBlockCoords = blockLookedAt;
            switch (faceLookedAt) {
                case FACE_TOP:
                    printf("top\n");
                    newBlockCoords.y += 1;
                    break;
                case FACE_BOTTOM:
                    printf("bottom\n");
                    newBlockCoords.y -= 1;
                    break;
                case FACE_LEFT:
                    printf("left\n");
                    newBlockCoords.x += 1;
                    break;
                case FACE_RIGHT:
                    printf("right\n");
                    newBlockCoords.x -= 1;
                    break;
                case FACE_FRONT:
                    printf("front\n");
                    newBlockCoords.z += 1;
                    break;
                case FACE_BACK:
                    printf("back\n");
                    newBlockCoords.z -= 1;
                    break;
                case -1:
                    printf("Error\n");
            }
            World::instance->placeVoxel(newBlockCoords.x, newBlockCoords.y, newBlockCoords.z, TEXTURE_DEFAULT);
        }

        if (InputManager::getMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
            World::instance->destroyVoxel(blockLookedAt.x, blockLookedAt.y, blockLookedAt.z);
    }
};

#endif
