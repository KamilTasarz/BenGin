#pragma once

enum class InputKey {

	// Keyboard
	W, S, A, D, UP, DOWN, LEFT, RIGHT,
	
	// Mouse
	MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE, MOUSE_MOVE_X, MOUSE_MOVE_Y,
	
	// Gamepad
	GAMEPAD_TRIANGLE, GAMEPAD_SQUARE, GAMEPAD_X, GAMEPAD_CIRCLE,
	GAMEPAD_LEFT_STICK_X, GAMEPAD_LEFT_STICK_Y, GAMEPAD_RIGHT_STICK_X, GAMEPAD_RIGHT_STICK_Y

};

enum class InputSource {
	
	KEYBOARD, MOUSE, GAMEPAD, UNKNOWN

};

InputSource getInputSourceFromKey(InputKey key) {
	
	switch (key) {

		case InputKey::W:
		case InputKey::S:
		case InputKey::A:
		case InputKey::D:
		case InputKey::UP:
		case InputKey::DOWN:
		case InputKey::LEFT:
		case InputKey::RIGHT:
			return InputSource::KEYBOARD;

		case InputKey::MOUSE_LEFT:
		case InputKey::MOUSE_RIGHT:
		case InputKey::MOUSE_MIDDLE:
		case InputKey::MOUSE_MOVE_X:
		case InputKey::MOUSE_MOVE_Y:
			return InputSource::MOUSE;

		case InputKey::GAMEPAD_TRIANGLE:
		case InputKey::GAMEPAD_SQUARE:
		case InputKey::GAMEPAD_X:
		case InputKey::GAMEPAD_CIRCLE:
		case InputKey::GAMEPAD_LEFT_STICK_X:
		case InputKey::GAMEPAD_LEFT_STICK_Y:
		case InputKey::GAMEPAD_RIGHT_STICK_X:
		case InputKey::GAMEPAD_RIGHT_STICK_Y:
			return InputSource::GAMEPAD;

		default:
			return InputSource::UNKNOWN;

	}

}
