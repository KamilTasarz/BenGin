#pragma once

#ifndef SERVICE_LOCATOR_H
#define SERVICE_LOCATOR_H

#include <memory>

#include "../Input/InputManager.h"
//#include "../Input/Input.h"
#include "Window.h"
#include "../AudioEngine.h"
//#include "PhysicsSystem.h"

//class Window;
//class InputManager;

class ServiceLocator {

public:
	
	static inline InputManager* getInputManager() { return _input_manager.get(); }
	static inline Window* getWindow() { return _window.get(); }
	static inline CAudioEngine* getAudioEngine() { return _audio_engine.get(); }
	//static inline PhysicsSystem* getPhysicsSystem() { return _system.get(); }

	static inline void provide(InputManager* input_manager) {
		if (_input_manager != nullptr) { return; }
		_input_manager = std::unique_ptr<InputManager>(input_manager);
	}

	static inline void provide(Window* window) {
		if (_window != nullptr) { return; }
		_window = std::unique_ptr<Window>(window);
	}

	static inline void provide(CAudioEngine* audio_engine) {
		if (_audio_engine != nullptr) { return; }
		_audio_engine = std::unique_ptr<CAudioEngine>(audio_engine);
	}

	/*static inline void provide(PhysicsSystem* system) {
		if (_system != nullptr) { return; }
		_system = std::unique_ptr<PhysicsSystem>(system);
	}*/

	static inline void shutdownServices() {
		shutdownAudioEngine();
		shutdownInputManager();
		shutdownWindow();
		//shutdownPhysics();
	}

private:

	static inline std::unique_ptr<InputManager> _input_manager = nullptr;
	static inline std::unique_ptr<Window> _window = nullptr;
	static inline std::unique_ptr<CAudioEngine> _audio_engine = nullptr;
	//static inline std::unique_ptr<PhysicsSystem> _system = nullptr;

	static inline void shutdownInputManager() {
		if (!_input_manager) { return; }
		_input_manager.reset(); // Makes a nullptr
	}

	static inline void shutdownWindow() {
		if (!_window) { return; }
		_window.reset(); // Makes a nullptr
	}

	static inline void shutdownAudioEngine() {
		if (!_audio_engine) { return; }
		_audio_engine.get()->Shutdown();
		_audio_engine.reset(); // Makes a nullptr
	}

	//static inline void shutdownPhysics() {
	//	if (!_system) { return; }
	//	_system.reset(); // Makes a nullptr
	//}

};

#endif // !SERVICE_LOCATOR_H
