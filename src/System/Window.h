#pragma once

#ifndef WINDOW_H
#define WINDOW_H

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#include "../config.h"
#include "../Input/InputKey.h"
#include "../Input/InputDevice.h"
#include "../Input/InputManager.h"
#include "../Input/Input.h"

#include <memory>

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double posX, double posY);
void leftClick(float value);



class Window {

private:

	std::unordered_map<InputKey, InputDeviceState> getGamepadState(int joystick_id);

	//Input _input {};

public:

	bool is_fullscreen = false;

    bool is_camera = true, is_camera_prev = false;
    bool mouse_pressed = false;

    bool firstMouseMovement = true;

    float lastX = (float)WINDOW_WIDTH / 2.0;
    float lastY = (float)WINDOW_HEIGHT / 2.0;

    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

	int windowPosX, windowPosY;
	int width, height;
	const char* title;
	GLFWwindow* window;
	GLFWmonitor* monitor;

	Window(float _width, float _height, const char* _title) {
		this->width = _width;
		this->height = _height;
		this->title = _title;
		initializeWindow();
	}

	~Window() {
		glfwTerminate();
	}

	void initializeWindow();

	void updateWindow() {
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	void toggleFullscreen();

};



#endif // !WINDOW_H
