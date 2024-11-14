#ifndef PLAYER_H
#define PLAYER_H

#define MAX_RANGE 4
#define MOUSE_SENSITIVITY 1

#include "Camera.h"
#include "World.h"

#include "AABB.h"
#include "Collisions.h"

class Player {
    Camera camera;

    glm::vec3 blockLookedAt = {-1, -1, -1};
    FaceOrientation faceLookedAt = FACE_TOP;

    float movementSpeed = 15;
    float fallSpeed = 1;

    static AABB getPlayerAABB(glm::vec3 position);

    static AABB getBlockAABB(glm::vec3 blockCoords);

    static bool isCollidingWithNearBlocks(glm::vec3 pos);

    void getBlockLookedAt();

    void lookAround(glm::vec2 mouseDelta);

public:
    explicit Player(glm::vec3 position = {0, 7, 0});

    [[nodiscard]] const Camera* getCamera() const;

    void Update(float deltaTime);
};

#endif
