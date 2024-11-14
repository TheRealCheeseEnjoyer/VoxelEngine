#include "../include/Player.h"

AABB Player::getPlayerAABB(const glm::vec3 position) {
    return {
        .minX = position.x -.25f,
        .maxX = position.x + .25f,
        .minY = position.y - 1.75f,
        .maxY = position.y + .25f,
        .minZ = position.z - .25f,
        .maxZ = position.z + .25f,
    };
}

AABB Player::getBlockAABB(const glm::vec3 blockCoords) {
    return {
        .minX = blockCoords.x - .5f,
        .maxX = blockCoords.x + .5f,
        .minY = blockCoords.y - .5f,
        .maxY = blockCoords.y + .5f,
        .minZ = blockCoords.z - .5f,
        .maxZ = blockCoords.z + .5f,
        };
}

bool Player::isCollidingWithNearBlocks(const glm::vec3 pos) {
    for (int x = -1; x <= 1; x++) {
        for (int y = -2; y <= 0; y++) {
            for (int z = -1; z <= 1; z++) {
                glm::vec3 blockPos = {round(pos.x + x), round(pos.y + y), round(pos.z + z)};
                Voxel* voxel = World::instance->getVoxel(blockPos);
                if (voxel == nullptr || voxel->type == TEXTURE_NONE)
                    continue;

                AABB blockAABB = getBlockAABB(blockPos);
                AABB playerAABB = getPlayerAABB(pos);
                if (Collisions::AABBToAABB(playerAABB, blockAABB)) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Player::getBlockLookedAt() {
    blockLookedAt = {-1, -1, -1};

    glm::vec3 currentBlockCoords = {round(camera.Position.x), round(camera.Position.y), round(camera.Position.z)};
    float minDistance = INFINITY;
    for (int x = -MAX_RANGE; x <= MAX_RANGE; x++) {
        for (int y = -MAX_RANGE; y <= MAX_RANGE; y++) {
            for (int z = -MAX_RANGE; z <= MAX_RANGE; z++) {
                glm::vec3 blockCoords = {currentBlockCoords.x + x, currentBlockCoords.y + y, currentBlockCoords.z + z};

                Voxel* voxel = World::instance->getVoxel(blockCoords.x, blockCoords.y, blockCoords.z);
                if (voxel == nullptr || voxel->type == TEXTURE_NONE)
                    continue;

                float distance;
                FaceOrientation face;
                AABB aabbBox = getBlockAABB(blockCoords);
                if (Collisions::rayToAABB(camera.Position, camera.Front, aabbBox, distance, face)) {
                    if (distance > MAX_RANGE) continue;
                    if (distance < minDistance) {
                        faceLookedAt = face;
                        minDistance = distance;
                        blockLookedAt = blockCoords;
                    }
                }
            }
        }
    }
}

void Player::lookAround(glm::vec2 mouseDelta) {
    float xOffset = mouseDelta.x * MOUSE_SENSITIVITY;
    float yOffset = mouseDelta.y * MOUSE_SENSITIVITY;

    float newYaw = camera.Yaw + xOffset;
    float newPitch = camera.Pitch + yOffset;

    if (newPitch > 89.9f)
        newPitch = 89.9f;
    if (newPitch < -89.9f)
        newPitch = -89.9f;

    camera.Yaw = newYaw;
    camera.Pitch = newPitch;
}

Player::Player(glm::vec3 position) : camera(position) {}

const Camera * Player::getCamera() const  { return &camera; }

void Player::Update(const float deltaTime) {
        glm::vec3 newPos = camera.Position;
        auto mouseDelta = InputManager::getMouseDelta();
        lookAround(mouseDelta);

        auto input = InputManager::getMovementInput();
        float velocity = movementSpeed * deltaTime;
        glm::vec3 cameraRightDisplacement = input.x * velocity * camera.Right;
        glm::vec3 cameraFrontDisplacement = input.y * velocity * normalize(glm::vec3(camera.Front.x, 0, camera.Front.z));
        glm::vec3 displacement = cameraFrontDisplacement + cameraRightDisplacement;

        newPos.x += displacement.x;
        if (isCollidingWithNearBlocks(newPos))
            newPos = camera.Position;

        glm::vec3 tempPos = newPos;
        newPos.z += displacement.z;

        if (isCollidingWithNearBlocks(newPos))
            newPos = tempPos;

        tempPos = newPos;
        newPos.y -= fallSpeed * deltaTime;

        if (isCollidingWithNearBlocks(newPos))
            newPos = tempPos;

        camera.Position = newPos;

        getBlockLookedAt();

        if (InputManager::getMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) {
            glm::vec3 newBlockCoords = blockLookedAt;
            switch (faceLookedAt) {
                case FACE_TOP:
                    newBlockCoords.y += 1;
                    break;
                case FACE_BOTTOM:
                    newBlockCoords.y -= 1;
                    break;
                case FACE_LEFT:
                    newBlockCoords.x += 1;
                    break;
                case FACE_RIGHT:
                    newBlockCoords.x -= 1;
                    break;
                case FACE_FRONT:
                    newBlockCoords.z += 1;
                    break;
                case FACE_BACK:
                    newBlockCoords.z -= 1;
                    break;
            }
            World::instance->placeVoxel(newBlockCoords.x, newBlockCoords.y, newBlockCoords.z, TEXTURE_DEFAULT);
        }

        if (InputManager::getMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
            World::instance->destroyVoxel(blockLookedAt.x, blockLookedAt.y, blockLookedAt.z);

        camera.updateVectors();
    }
