#include "../include/Collisions.h"

bool Collisions::rayToAABB(const glm::vec3 origin, const glm::vec3 direction, const AABB &aabb, float &distance, FaceOrientation &face) {
    float tMin = -INFINITY, tMax = INFINITY;

    FaceOrientation yFace = (FaceOrientation)-1, xFace = (FaceOrientation)-1, zFace = (FaceOrientation)-1;
    if (origin.y > aabb.maxY)
        yFace = FACE_TOP;
    else if (origin.y < aabb.minY)
        yFace = FACE_BOTTOM;

    if (origin.x > aabb.maxX)
        xFace = FACE_LEFT;
    else if (origin.x < aabb.minX)
        xFace = FACE_RIGHT;

    if (origin.z > aabb.maxZ)
        zFace = FACE_FRONT;
    else if (origin.z < aabb.minZ)
        zFace = FACE_BACK;

    float tx1 = (aabb.minX - origin.x) / direction.x;
    float tx2 = (aabb.maxX - origin.x) / direction.x;

    tMin = fmax(tMin, fmin(tx1, tx2));
    tMax = fmin(tMax, fmax(tx1, tx2));

    float ty1 = (aabb.minY - origin.y) / direction.y;
    float ty2 = (aabb.maxY - origin.y) / direction.y;

    tMin = fmax(tMin, fmin(ty1, ty2));
    tMax = fmin(tMax, fmax(ty1, ty2));

    float tz1 = (aabb.minZ - origin.z) / direction.z;
    float tz2 = (aabb.maxZ - origin.z) / direction.z;

    tMin = fmax(tMin, fmin(tz1, tz2));
    tMax = fmin(tMax, fmax(tz1, tz2)),

    distance = tMin;
    glm::vec3 ray = origin + direction * distance;
    if (ray.y > aabb.minY && ray.y < aabb.maxY && ray.z > aabb.minZ && ray.z < aabb.maxZ) {
        face = xFace;
    } else if (ray.y > aabb.minY && ray.y < aabb.maxY && ray.x > aabb.minX && ray.x < aabb.maxX) {
        face = zFace;
    } else if (ray.x > aabb.minX && ray.x < aabb.maxX && ray.z > aabb.minZ && ray.z < aabb.maxZ) {
        face = yFace;
    }

    return tMin <= tMax && tMax > 0;
}

bool Collisions::AABBToAABB(const AABB &aabb1, const AABB &aabb2) {
    return  aabb1.minX <= aabb2.maxX &&
            aabb1.maxX >= aabb2.minX &&
            aabb1.minY <= aabb2.maxY &&
            aabb1.maxY >= aabb2.minY &&
            aabb1.minZ <= aabb2.maxZ &&
            aabb1.maxZ >= aabb2.minZ;
}
