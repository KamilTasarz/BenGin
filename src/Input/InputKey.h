#pragma once

#include <string>

enum class InputKey {

	UNKNOWN,

	// Keyboard
	W, S, A, D, UP, DOWN, LEFT, RIGHT,
	
	// Mouse
	MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE, MOUSE_POSX, MOUSE_POSY, MOUSE_MOVE_X, MOUSE_MOVE_Y,
	
	// Gamepad
	GAMEPAD_TRIANGLE, GAMEPAD_SQUARE, GAMEPAD_X, GAMEPAD_CIRCLE,
	GAMEPAD_LEFT_STICK_X, GAMEPAD_LEFT_STICK_Y, GAMEPAD_RIGHT_STICK_X, GAMEPAD_RIGHT_STICK_Y

};

enum class InputSource {
	
	KEYBOARD, MOUSE, GAMEPAD, UNKNOWN

};

struct InputAction {

	std::string action_name {""};

	float scale {1.0f};

};

InputSource getInputSourceFromKey(InputKey key);
