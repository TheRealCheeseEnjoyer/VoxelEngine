#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "AABB.h"
#include <glm/vec3.hpp>
#include "Chunk.h"

class Collisions {
public:
    Collisions() = delete;
    static bool rayToAABB(glm::vec3 origin, glm::vec3 direction, const AABB &aabb, float& distance, FaceOrientation& face);

    static bool AABBToAABB(const AABB& aabb1, const AABB& aabb2);
};



#endif //COLLISIONS_H
