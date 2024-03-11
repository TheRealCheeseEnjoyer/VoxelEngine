#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "src/Shader.h"
#include "src/stb_image.h"
#include "src/InputManager.h"
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return window;
}

std::vector<std::vector<float>> positions;

/*
 *      4 ----- 7
 * 5 ------ 6   |
 * |    |   |   |
 * |    0 --|-- 3
 * 1 ------ 2
 */

/*
 * up, down, left, right, front, back are collections of vertices that describe a certain face.
 * there are 2^6 = 64 different variations of a cube, depending on how many and which faces have to be drawn
 * we use a normal integer to store this information about every voxel, like this
 * 00|100101
 * the first 2 bits are unused
 * the rest of the bits, in order from right to left, represent up, down, left, right, front, back,
 * a 1 bit means that the face should be sent to render, while a 0 means that we should not draw it.
 * This way each voxel checks if its faces are covered by other solid blocks, and modify the number accordingly.
 * This number will be used to load in a series of indexes, that an EBO will use to only draw the faces
 * that we want to.
 * The EBO table is computed at compile time.
 * e.g., the example above represents a cube with only the up, left and back faces rendered.
 * NOTE: This only represents faces not obstructed by near blocks, but they could still be culled away by OpenGL.
 * ngl this is kinda genius
 */
constexpr std::array<unsigned int, 6> up = {
        4, 5, 6, 4, 6, 7
};

constexpr std::array<unsigned int, 6> down = {
        0, 1, 2, 0, 2, 3
};

constexpr std::array<unsigned int, 6> left = {
        0, 1, 5, 0, 5, 4
};

constexpr std::array<unsigned int, 6> right = {
        3, 2, 6, 3, 6, 7
};

constexpr std::array<unsigned int, 6> front = {
        1, 2, 5, 2, 5, 6
};

constexpr std::array<unsigned int, 6> back = {
        3, 0, 4, 3, 4, 7
};

constexpr std::array<std::array<unsigned int, 6>, 6> faces = {
        up, down, left, right, front, back
};

constexpr auto calculateIndexes() {
    std::array<std::vector<std::array<unsigned int, 6>>, 64> out{};
    std::vector<std::array<unsigned int, 6>> indexes;
    for (int i = 0; i < 64; i++) {
        for (int mask = 1, pos = 0; pos < 6; ++pos) {
            if ((mask & i) == 1) {
                indexes.push_back(faces[pos]);
            }

            mask <<= 1;
        }
    }
    return out;
}

constexpr auto indexes = calculateIndexes();

float verticesEBO[24] = {
        -.5f, -.5f, -.5f,
        .5f, -.5f, -.5f,
        .5f, -.5f, .5f,
        -.5f, -.5f, .5f,

        -.5f, .5f, -.5f,
        .5f, .5f, -.5f,
        .5f, .5f, .5f,
        -.5f, .5f, .5f,
};

float vertices[180] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
};

int main() {
    GLFWwindow* window = init();

    InputManager* inputManager = InputManager::getInstance();
    inputManager->registerKey(GLFW_KEY_W);
    inputManager->registerKey(GLFW_KEY_A);
    inputManager->registerKey(GLFW_KEY_S);
    inputManager->registerKey(GLFW_KEY_D);
    inputManager->registerKey(GLFW_KEY_ESCAPE);
    inputManager->registerButton(GLFW_MOUSE_BUTTON_LEFT);
    inputManager->registerButton(GLFW_MOUSE_BUTTON_RIGHT);

    for (int i = -49; i <= 50; ++i) {
        for (int j = -49; j <= 50; j++) {

        }
    }


    Shader shader;
    std::array<unsigned int, 64> ebos{};
    unsigned int VAO, VBO, texture;


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(64, ebos.data());

    glBindVertexArray(VAO);

    /*glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);*/

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesEBO), verticesEBO, GL_STATIC_DRAW);

    for (int i = 0; i < 64; i++) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebos[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)indexes[i].size(), indexes[i].data(), GL_STATIC_DRAW);
    }

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

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
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

    float deltaTime;    // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame
    while (!glfwWindowShouldClose(window)) {
        auto currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float cameraSpeed = 2.5f * deltaTime;
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

                for (auto iter = std::begin(positions); iter != positions.end(); iter++) {
                    if (*iter == pointedTo) {
                        positions.erase(iter);
                        found = true;
                        break;
                    }
                }
                if (found)
                    break;
            }
        }

        if (inputManager->getMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) {
            glm::vec3* block = nullptr;
            for (int c = 1; c <= MAXIMUM_REACH; c++) {
                glm::vec3 pointedTo = cameraPos + glm::vec3(c) * cameraFront;
                pointedTo = glm::vec3(roundf(pointedTo.x), roundf(pointedTo.y), roundf(pointedTo.z));

                for (auto iter = std::begin(positions); iter != positions.end(); iter++) {
                    if (*iter == pointedTo) {
                        std::cout << iter->x << " " << iter->z << std::endl;
                        block = iter.base();
                        break;
                    }
                }
                if (block != nullptr)
                    break;
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

        for (auto position: positions) {
            auto transform = glm::mat4(1);
            transform = glm::translate(transform, position);

            unsigned int ebo;

            shader.setMat4("model", transform);
            shader.setMat4("view", view);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glDrawElements(GL_TRIANGLES, )
        }

        inputManager->resetInput();
        glfwSwapBuffers(window);
        glfwPollEvents();
        std::cout << "Frame time: " << glfwGetTime() - currentFrame << std::endl;
    }

    glfwTerminate();

    return 0;
}