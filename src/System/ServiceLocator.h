#pragma once

#ifndef SERVICE_LOCATOR_H
#define SERVICE_LOCATOR_H

#include <memory>

#include "../Input/InputManager.h"
//#include "../Input/Input.h"
#include "Window.h"

//class Window;
//class InputManager;

class ServiceLocator {

public:

	static inline InputManager* getInputManager() { return _input_manager.get(); }
	static inline Window* getWindow() { return _window.get(); }

	static inline void provide(InputManager* input_manager) {
		if (_input_manager != nullptr) { return; }
		_input_manager = std::unique_ptr<InputManager>(input_manager);
	}

	static inline void provide(Window* window) {
		if (_window != nullptr) { return; }
		_window = std::unique_ptr<Window>(window);
	}

	static inline void shutdownServices() {
		shutdownInputManager();
		shutdownWindow();
	}

private:

	static inline std::unique_ptr<InputManager> _input_manager = nullptr;
	static inline std::unique_ptr<Window> _window = nullptr;

	static inline void shutdownInputManager() {
		if (!_input_manager) { return; }
		_input_manager.reset(); // Makes a nullptr
	}

	static inline void shutdownWindow() {
		if (!_window) { return; }
		_window.reset(); // Makes a nullptr
	}

};

#endif // !SERVICE_LOCATOR_H
