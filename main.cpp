#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "src/Shader.h"
#include "src/stb_image.h"
#include "src/InputManager.h"
#include "src/World.h"
#include <vector>
#include <array>

#define MAXIMUM_REACH 5
#define SCR_WIDTH 1280
#define SCR_HEIGHT 720

GLFWwindow* init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Heisencraft", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(1);
    }
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



    return window;
}

int main() {
    GLFWwindow* window = init();
    glfwSwapInterval(0);

    World world;

    InputManager* inputManager = InputManager::getInstance();
    inputManager->registerKey(GLFW_KEY_W);
    inputManager->registerKey(GLFW_KEY_A);
    inputManager->registerKey(GLFW_KEY_S);
    inputManager->registerKey(GLFW_KEY_D);
    inputManager->registerKey(GLFW_KEY_ESCAPE);
    inputManager->registerButton(GLFW_MOUSE_BUTTON_LEFT);
    inputManager->registerButton(GLFW_MOUSE_BUTTON_RIGHT);


    Shader shader;
    unsigned int VAO, VBO, texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("assets/image.png", &width, &height, &nrChannels, 0);

    shader.use();

    if (data != nullptr) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);

    auto model = glm::mat4(1),
            projection = glm::mat4(1);

    float yaw = -90, pitch = 0;
    glm::vec3 direction;
    direction.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
    direction.y = sinf(glm::radians(pitch));
    direction.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraFront = glm::normalize(direction);

    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(.2f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
    // pass transformation matrices to the shader
    shader.setMat4("model", model);
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader.use();
    shader.setInt("texture1", 0);

    std::cout << texture << std::endl;

    float deltaTime;    // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame
    while (!glfwWindowShouldClose(window)) {
        auto currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float cameraSpeed = 10 * deltaTime;
        inputManager->updateInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (inputManager->getKeyDown(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);

        if (inputManager->getKey(GLFW_KEY_W)) {
            cameraPos += cameraSpeed * cameraFront;
        } else if (inputManager->getKey(GLFW_KEY_S)) {
            cameraPos -= cameraSpeed * cameraFront;
        }

        if (inputManager->getKey(GLFW_KEY_A)) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        } else if (inputManager->getKey(GLFW_KEY_D)) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }

        if (inputManager->getMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
            for (int c = 1; c <= MAXIMUM_REACH; c++) {
                bool found = false;
                glm::vec3 pointedTo = cameraPos + glm::vec3(c) * cameraFront;
                pointedTo = glm::vec3(roundf(pointedTo.x), roundf(pointedTo.y), roundf(pointedTo.z));

                //world.remove(pointedTo);
            }
        }

        if (inputManager->getMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) {
            glm::vec3* block = nullptr;
            for (int c = 1; c <= MAXIMUM_REACH; c++) {
                glm::vec3 pointedTo = cameraPos + glm::vec3(c) * cameraFront;
                pointedTo = glm::vec3(roundf(pointedTo.x), roundf(pointedTo.y), roundf(pointedTo.z));

                //auto voxel = world.get(pointedTo);
            }

            if (block != nullptr) {
                glm::vec3 topRightVertex = *block + 0.5f;
                glm::vec3 bottomLeftVertex = glm::vec3(block->x - 0.5f, block->y + 0.5f, block->z - 0.5f);

                glm::vec3 dir = cameraPos + cameraFront;

                if (glm::dot(glm::cross(bottomLeftVertex, dir), glm::cross(bottomLeftVertex, topRightVertex)) >= 0
                    && glm::dot(glm::cross(topRightVertex, dir), glm::cross(topRightVertex, bottomLeftVertex)) >= 0) {
                    std::cout << "top face" << std::endl;
                }

            }
        }


        glm::vec2 mouseDelta = inputManager->getMouseDelta();
        yaw += mouseDelta.x;
        pitch += mouseDelta.y;
        if (pitch > 89) {
            pitch = 89;
        } else if (pitch < -89) {
            pitch = -89;
        }

        direction.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
        direction.y = sinf(glm::radians(pitch));
        direction.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);


        glBindVertexArray(VAO);
        world.draw(view, projection);

        glfwPollEvents();
        inputManager->resetInput();
        glfwSwapBuffers(window);
        glFinish();
        std::cout << "FPS: " << 1/(glfwGetTime() - currentFrame) << std::endl;
    }

    glfwTerminate();

    return 0;
}