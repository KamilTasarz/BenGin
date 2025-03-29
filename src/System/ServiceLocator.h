#pragma once

#ifndef SERVICE_LOCATOR_H
#define SERVICE_LOCATOR_H

#include <memory>

#include "../Input/InputManager.h"

class ServiceLocator {

public:

	static inline InputManager* getInputManager() { return _input_manager.get(); }

	static inline void provide(InputManager* input_manager) {
		if (_input_manager != nullptr) { return; }
		_input_manager = std::unique_ptr<InputManager>(input_manager);
	}

	static inline void shutdownServices() {
		shutdownInputManager();
	}

private:

	static inline std::unique_ptr<InputManager> _input_manager = nullptr;

	static inline void shutdownInputManager() {
		if (!_input_manager) { return; }
		_input_manager.reset(); // Makes a nullptr
	}

};

#endif // !SERVICE_LOCATOR_H
