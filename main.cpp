#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "include/stb_image.h"
#include "include/InputManager.h"
#include "include/World.h"
#include "include/Camera.h"
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <cctype>

#include "include/Player.h"

#define SCR_WIDTH 1280
#define SCR_HEIGHT 720

bool debuggerIsAttached();

GLFWwindow* init();

int main() {
    GLFWwindow* window = init();
    ThreadPool::Start();

    Player player;
    const Camera* playerCamera = player.getCamera();
    World world(*playerCamera);

    InputManager::registerKey(GLFW_KEY_W);
    InputManager::registerKey(GLFW_KEY_A);
    InputManager::registerKey(GLFW_KEY_S);
    InputManager::registerKey(GLFW_KEY_D);
    InputManager::registerKey(GLFW_KEY_SPACE);
    InputManager::registerKey(GLFW_KEY_ESCAPE);
    InputManager::registerButton(GLFW_MOUSE_BUTTON_LEFT);
    InputManager::registerButton(GLFW_MOUSE_BUTTON_RIGHT);
    InputManager::registerButton(GLFW_MOUSE_BUTTON_MIDDLE);

    float lastFrame = 0.0f; // Time of last frame
    while (!glfwWindowShouldClose(window)) {
        InputManager::updateInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto currentFrame = (float) glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        player.Update(deltaTime);

        if (InputManager::getKeyDown(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);

        world.updateLoadedChunks(*playerCamera);
        world.draw(*playerCamera);

        glfwPollEvents();
        InputManager::resetInput();
        glfwSwapBuffers(window);
        glFinish();
        //std::cout << "FPS: " << 1 / (glfwGetTime() - currentFrame) << std::endl;
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    ThreadPool::Stop();
    return 0;
}

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
    if (!debuggerIsAttached())
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}

bool debuggerIsAttached() {
    char buf[4096];

    const int status_fd = open("/proc/self/status", O_RDONLY);
    if (status_fd == -1)
        return false;

    const ssize_t num_read = read(status_fd, buf, sizeof(buf) - 1);
    close(status_fd);

    if (num_read <= 0)
        return false;

    buf[num_read] = '\0';
    constexpr char tracerPidString[] = "TracerPid:";
    const auto tracer_pid_ptr = strstr(buf, tracerPidString);
    if (!tracer_pid_ptr)
        return false;

    for (const char* characterPtr = tracer_pid_ptr + sizeof(tracerPidString) - 1;
         characterPtr <= buf + num_read; ++characterPtr) {
        if (isspace(*characterPtr))
            continue;
        else
            return isdigit(*characterPtr) != 0 && *characterPtr != '0';
         }

    return false;
}