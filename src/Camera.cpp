#include "../include/Camera.h"

Camera::Camera(glm::vec3 position, float yaw, float pitch) : Position(position), Yaw(yaw), Pitch(pitch) {
    updateVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::GetProjectionMatrix() const {
    return glm::perspective(Fov, (float)ScrWidth / (float)ScrHeight, .1f, 1000.f);
}

glm::mat4 Camera::GetMatrices() const {
    return GetProjectionMatrix() * GetViewMatrix();
}

void Camera::updateVectors() {
    glm::vec3 front;
    front.x = cosf(glm::radians(Yaw)) * cosf(glm::radians(Pitch));
    front.y = sinf(glm::radians(Pitch));
    front.z = sinf(glm::radians(Yaw)) * cosf(glm::radians(Pitch));
    Front = normalize(front);
    Right = normalize(glm::cross(Front, WorldUp));
    Up = normalize(glm::cross(Right, Front));
}
