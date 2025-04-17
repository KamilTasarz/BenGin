#include <GLFW/glfw3.h>

#include "Input.h"

void Input::updateKeyboardState(int key, float value) {

	InputKey input_key = keyToInputKey(key);

	_keyboard_state[input_key].value = value;

}

void Input::updateMouseState(int button, float value) {

	InputKey input_key = mouseButtonToInputKey(button);

	_mouse_state[input_key].value = value;

}

InputKey Input::keyToInputKey(int key) {

	// Thanks to this we can provide different backends if we just define everything as an InputKey

	switch (key) {
	
		case GLFW_KEY_A:
			return InputKey::A;
		case GLFW_KEY_D:
			return InputKey::D;
		case GLFW_KEY_S:
			return InputKey::S;
		case GLFW_KEY_W:
			return InputKey::W;

		default:
			return InputKey::UNKNOWN;
	
	}

}

InputKey Input::mouseButtonToInputKey(int button) {

	switch (button) {
	
		case GLFW_MOUSE_BUTTON_LEFT:
			return InputKey::MOUSE_LEFT;
		case GLFW_MOUSE_BUTTON_RIGHT:
			return InputKey::MOUSE_RIGHT;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			return InputKey::MOUSE_MIDDLE;

		default:
			return InputKey::UNKNOWN;

	}

}
