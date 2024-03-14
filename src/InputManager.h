#ifndef ASTEROIDS2_INPUTMANAGER_H
#define ASTEROIDS2_INPUTMANAGER_H

#include <memory>
#include <unordered_map>
#include <memory>
#include <GLFW/glfw3.h>

struct Status {
    bool isPressed = false;
    bool wasPressed = false;
};

struct Mouse {
    glm::vec2 lastPos = glm::vec2(640, 360);
    glm::vec2 offset;
};

enum Axis {
    HORIZONTAL,
    VERTICAL
};

class InputManager {
private:
    static std::unique_ptr<InputManager> instance;
    std::unordered_map<int, Status> keys;
    std::unordered_map<int, Status> buttons;

    Mouse mouse;

public:
    static InputManager* getInstance() {
        if (instance == nullptr) {
            instance = std::make_unique<InputManager>();
        }

        return instance.get();
    }

    void registerKey(int key) {
        if (keys.contains(key))
            return;

        keys.insert({key, Status()});
    };

    void registerButton(int button) {
        if (buttons.contains(button))
            return;

        buttons.insert({button, Status()});
    }

    glm::vec2 getMovementInput() {
        int x = 0, y = 0;
        if (getKey(GLFW_KEY_W))
            y += 1;
        if (getKey(GLFW_KEY_S))
            y -= 1;
        if (getKey(GLFW_KEY_A))
            x -= 1;
        if (getKey(GLFW_KEY_D))
            x += 1;

        if (x == 0 && y == 0)
            return {x, y};

        return glm::normalize(glm::vec2(x, y));
    }

    bool getKeyDown(int key) {
        if (!keys.contains(key))
            return false;

        return keys[key].isPressed && !keys[key].wasPressed;
    }

    bool getKey(int key) {
        if (!keys.contains(key))
            return false;

        return keys[key].isPressed;
    }

    bool getKeyUp(int key) {
        if (!keys.contains(key))
            return false;

        return !keys[key].isPressed && keys[key].wasPressed;
    }

    bool getMouseButtonDown(int button) {
        if (!buttons.contains(button))
            return false;

        return buttons[button].isPressed && !buttons[button].wasPressed;
    }

    bool getMouseButton(int button) {
        if (!buttons.contains(button))
            return false;

        return buttons[button].isPressed;
    }

    bool getMouseButtonUp(int button) {
        if (!buttons.contains(button))
            return false;

        return !buttons[button].isPressed && buttons[button].wasPressed;
    }

    glm::vec2 getMouseDelta() const {
        return mouse.offset;
    }

    void updateInput(GLFWwindow* window) {
        for (auto& [key, status] : keys) {
            status.isPressed = glfwGetKey(window, key) == GLFW_PRESS;
        }

        for (auto& [button, status] : buttons) {
            status.isPressed = glfwGetMouseButton(window, button) == GLFW_PRESS;
        }


        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        mouse.offset = glm::vec2(xPos - mouse.lastPos.x, mouse.lastPos.y - yPos);
        mouse.lastPos = glm::vec2(xPos, yPos);

        const float sensitivity = 0.1f;
        mouse.offset *= sensitivity;

    }

    void resetInput() {
        for (auto& [key, status] : keys) {
            status.wasPressed = status.isPressed;
            status.isPressed = false;
        }

        for (auto& [button, status] : buttons) {
            status.wasPressed = status.isPressed;
            status.isPressed = false;
        }
    }
};

std::unique_ptr<InputManager> InputManager::instance = nullptr;

#endif
