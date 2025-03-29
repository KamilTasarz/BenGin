#pragma once

#include <functional>

#include "InputKey.h"

enum class InputDeviceType {

	KEYBOARD, MOUSE, GAMEPAD

};

struct InputDeviceState {

	float value;

};

// Returns a map, takes an index
using InputDeviceStateCallbackFunction = std::function<std::unordered_map<InputKey, InputDeviceState>(int)>;

// GLFW will register an input device with a type, index, and provide a function to get the state of the device
struct InputDevice {

	InputDeviceType type;
	int index;
	std::unordered_map<InputKey, InputDeviceState> current_state;
	InputDeviceStateCallbackFunction stateFunction;

};
