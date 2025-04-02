#pragma once

#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

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

#endif // !WINDOW_H
