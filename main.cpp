#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "src/stb_image.h"
#include "src/InputManager.h"
#include "src/World.h"
#include "src/Camera.h"

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
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
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

    World::init();
    Camera camera({0, 0, -1}, {0, 1, 0}, 90, 0);

    InputManager* inputManager = InputManager::getInstance();
    inputManager->registerKey(GLFW_KEY_W);
    inputManager->registerKey(GLFW_KEY_A);
    inputManager->registerKey(GLFW_KEY_S);
    inputManager->registerKey(GLFW_KEY_D);
    inputManager->registerKey(GLFW_KEY_ESCAPE);
    inputManager->registerButton(GLFW_MOUSE_BUTTON_LEFT);
    inputManager->registerButton(GLFW_MOUSE_BUTTON_RIGHT);

    float deltaTime;    // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame
    while (!glfwWindowShouldClose(window)) {
        auto currentFrame = (float) glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        inputManager->updateInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto mouseDelta= inputManager->getMouseDelta();
        camera.ProcessMouseMovement(mouseDelta.x, mouseDelta.y);

        auto direction = inputManager->getMovementInput();
        camera.ProcessKeyboard(direction, deltaTime);

        if (inputManager->getKeyDown(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);

        World::draw(camera.GetMatrices());

        glfwPollEvents();
        inputManager->resetInput();
        glfwSwapBuffers(window);
        glFinish();
        std::cout << "FPS: " << 1 / (glfwGetTime() - currentFrame) << std::endl;
    }

    glfwTerminate();

    return 0;
}