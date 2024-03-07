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

class InputManager {
private:
    static std::unique_ptr<InputManager> instance;
    std::unordered_map<int, Status> keys;
    std::unordered_map<int, Status> buttons;

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

    void updateInput(GLFWwindow* window) {
        for (auto& [key, status] : keys) {
            status.isPressed = glfwGetKey(window, key) == GLFW_PRESS;
        }

        for (auto& [button, status] : buttons) {
            status.isPressed = glfwGetMouseButton(window, button) == GLFW_PRESS;
        }
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
