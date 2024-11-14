#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Default camera values
#define YAW (-90.0f)
#define PITCH 0.0f
#define FOV 45.0f
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
public:
    // camera Attributes
    glm::vec3 Position{};
    glm::vec3 Front{};
    glm::vec3 Up{};
    glm::vec3 Right{};
    glm::vec3 WorldUp = {0, 1, 0};
    // euler Angles
    float Yaw = YAW;
    float Pitch = PITCH;
    // camera options
    float Fov = FOV;
    int ScrWidth = SCREEN_WIDTH;
    int ScrHeight = SCREEN_HEIGHT;

    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

    [[nodiscard]]
    glm::mat4 GetViewMatrix() const;

    [[nodiscard]]
    glm::mat4 GetProjectionMatrix() const;

    [[nodiscard]]
    glm::mat4 GetMatrices() const;

    void updateVectors();
};

#endif

