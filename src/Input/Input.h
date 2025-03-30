#pragma once

#ifndef INPUT_H
#define INPUT_H

#include <unordered_map>

#include "InputKey.h"
#include "InputDevice.h"

class Input {

public:

	std::unordered_map<InputKey, InputDeviceState> getKeyboardState(int index) { return _keyboard_state; }
	std::unordered_map<InputKey, InputDeviceState> getMouseState(int index) { return _mouse_state; }
	std::unordered_map<InputKey, InputDeviceState> getGamepadState(int index) { return _gamepad_states[index]; }

	void updateKeyboardState(int key, float value);
	void updateMouseState(int button, float value);

private:

	std::unordered_map<InputKey, InputDeviceState> _keyboard_state {};
	std::unordered_map<InputKey, InputDeviceState> _mouse_state {};

	std::unordered_map<int, std::unordered_map<InputKey, InputDeviceState>> _gamepad_states {};

};

#endif // !INPUT_H
