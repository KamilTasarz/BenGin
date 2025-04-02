#pragma once

#ifndef WINDOW_H
#define WINDOW_H

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#include <GLFW/glfw3.h>

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

class Window {

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

#endif // !WINDOW_H
