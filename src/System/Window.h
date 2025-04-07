#pragma once

#ifndef WINDOW_H
#define WINDOW_H

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>

#include "src/System/ServiceLocator.h"

#include "src/Input/InputManager.h"
#include "src/Input/Input.h"


extern bool is_camera = true, is_camera_prev = false;
extern bool mouse_pressed = false;

extern bool firstMouseMovement = true;

extern float lastX = (float)WINDOW_WIDTH / 2.0;
extern float lastY = (float)WINDOW_HEIGHT / 2.0;

extern GLfloat deltaTime = 0.0f;
extern GLfloat lastFrame = 0.0f;

extern Camera* camera = new Camera(0.f, 0.f, -3.f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double posX, double posY);
void leftClick(float value);

class Window {

private:

    std::unordered_map<InputKey, InputDeviceState> getGamepadState(int joystick_id) {
    
        return std::unordered_map<InputKey, InputDeviceState> {};

    }

    Input _input {};

public:

	float width, height;
	const char* title;
	GLFWwindow* window;

	Window(float _width, float _height, const char* _title) {
		this->width = _width;
		this->height = _height;
		this->title = _title;
		initializeWindow();
	}

	~Window() {
		glfwTerminate();
		cout << "\n-----\nBAAAAJJJJLAAANDZIISKOOOOOO\n-----\n";
	}

	void initializeWindow() {
		
		if (!glfwInit()) {
			std::cerr << "Failed to initialize GLFW\n";
			// TODO: pomysl co z return -1 dla bezpieczenstwa
			// return -1;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_SAMPLES, 4);

		window = glfwCreateWindow(width, height, title, NULL, NULL);

		if (!window) {
			std::cerr << "Failed to create window\n";
			glfwTerminate();
			// TODO: pomysl co z return -1 dla bezpieczenstwa
			// return -1;
		}
		
		glfwMakeContextCurrent(window);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cerr << "Failed to initialize GLAD\n";
			// TODO: pomysl co z return -1 dla bezpieczenstwa
			// return -1;
		}

		glViewport(0, 0, width, height);

		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetCursorPosCallback(window, mouseCallback);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);

        // Gamma correction???
        glEnable(GL_FRAMEBUFFER_SRGB);

        // -- CULLING -- //

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glEnable(GL_MULTISAMPLE);

        auto input = std::make_shared<Input>();

        glfwSetWindowUserPointer(window, input.get());

        // KEYBOARD
        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {

            // Get the input
            auto* input = static_cast<Input*>(glfwGetWindowUserPointer(window));

            if (input) {

                // Set the new value for key
                float value = 0.0f;

                switch (action) {

                case GLFW_PRESS:
                case GLFW_REPEAT:
                    value = 1.0f;
                    break;

                    // GLFW_RELEASE + GLFW_UNKNOWN
                default:
                    value = 0.0f;

                }

                input->updateKeyboardState(key, value);

            }

            });

        // MOUSE
        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {

            // Get the input
            auto* input = static_cast<Input*>(glfwGetWindowUserPointer(window));

            if (input) {

                input->updateMouseState(button, action == GLFW_PRESS ? 1.0f : 0.0f);

            }

            });

        for (int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; i++) {
            glfwSetJoystickUserPointer(i, this);
            if (glfwJoystickPresent(i)) {
                std::cout << "Joystick " << i << " is present" << std::endl;
            }
        }

        // GAMEPAD
        glfwSetJoystickCallback([](int joystick_id, int event) {

            // Get our input manager
            auto* input_manager = ServiceLocator::getInputManager();

            if (glfwJoystickIsGamepad(joystick_id) && input_manager) {

                // Get the input
                auto* window = static_cast<Window*>(glfwGetJoystickUserPointer(joystick_id));

                if (window) {

                    if (event == GLFW_CONNECTED) {

                        input_manager->registerDevice(InputDevice{
                            .type = InputDeviceType::GAMEPAD,
                            .index = joystick_id,
                            .stateFunction = std::bind(&Window::getGamepadState, window, std::placeholders::_1)
                        });

                    }
                    else if (event == GLFW_DISCONNECTED) {

                        input_manager->removeDevice(InputDeviceType::GAMEPAD, joystick_id);

                    }

                }

            }

        });

        // Register our devices
        InputManager* _input_manager = new InputManager();

        ServiceLocator::provide(_input_manager);

        auto* input_manager = ServiceLocator::getInputManager();
        //auto* input_manager = ServiceLocator::getInputManager();

        if (input_manager) {

            input_manager->registerDevice(InputDevice{
                .type = InputDeviceType::KEYBOARD,
                .index = 0,
                .stateFunction = std::bind(&Input::getKeyboardState, input, std::placeholders::_1)
            });

            input_manager->registerDevice(InputDevice{
                .type = InputDeviceType::MOUSE,
                .index = 0,
                .stateFunction = std::bind(&Input::getMouseState, input, std::placeholders::_1)
            });

            // Key-action mapping

            input_manager->mapInputToAction(InputKey::A, InputAction{
                .action_name = "strafe",
                .scale = -1.0f
            });

            input_manager->mapInputToAction(InputKey::D, InputAction{
                .action_name = "strafe",
                .scale = 1.0f
            });

            input_manager->mapInputToAction(InputKey::S, InputAction{
                .action_name = "walk",
                .scale = -1.0f
            });

            input_manager->mapInputToAction(InputKey::W, InputAction{
                .action_name = "walk",
                .scale = 1.0f
            });

            // Mouse-action mapping

            input_manager->mapInputToAction(InputKey::MOUSE_LEFT, InputAction{
                .action_name = "left-click",
                .scale = 1.0f
            });

            // Action callbacks

            input_manager->registerActionCallback("strafe", InputManager::ActionCallback{
                .callback_reference = "Strafing is left and right",
                .function = [](InputSource source, int source_index, float value) {

                    std::string direction = "NONE";
                    if (value == 1.0f) direction = "RIGHT";
                    if (value == -1.0f) direction = "LEFT";
                    std::cout << "Strafing " << direction << "\n";
                    return true;

                }
                });

            input_manager->registerActionCallback("walk", InputManager::ActionCallback{
                .callback_reference = "Walking is front and back",
                .function = [](InputSource source, int source_index, float value) {
                    std::cout << (value != 0.0f ? (value == 1.0f ? "Walking FRONT\n" : "Walking BACKWARDS\n") : "");
                    return true;
                }
                });

            input_manager->registerActionCallback("left-click", InputManager::ActionCallback{
                .callback_reference = "Clicking left mouse button",
                .function = [](InputSource source, int source_index, float value) {
                    leftClick(value);
                    return true;
                }
                });

        }

	}

	void updateWindow() {
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	lastFrame = glfwGetTime();
}

void mouseCallback(GLFWwindow* window, double posX, double posY) {

	float x = static_cast<float>(posX);
	float y = static_cast<float>(posY);

	if (firstMouseMovement) {
		lastX = x;
		lastY = y;
		firstMouseMovement = false;
	}

	float offsetX = x - lastX;
	float offsetY = lastY - y; // reversed

	lastX = x;
	lastY = y;

	if (is_camera) {
		camera->ProcessMouseMovement(offsetX, offsetY);
	}

}

void leftClick(float value) {

    std::cout << "Clicked " << value << "\n";

}

#endif // !WINDOW_H
