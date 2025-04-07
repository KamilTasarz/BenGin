#include "InputManager.h"

// --- CONSTRUCTOR AND DESTRUCTOR --- //

InputManager::InputManager() {
	is_active = true;
	std::cout << "Input Manager provided\n";
}

InputManager::~InputManager() {
	is_active = false;
}

// --- REGISTER AND REMOVE ACTION CALLBACKS --- //

void InputManager::registerActionCallback(const std::string& action_name, const ActionCallback& callback) {

	// If you query a map and it doesn't exist, it's initialized by default
	// We emplace_back cause we are making a stack (we want callbacks at the back)
	// For example: space (on ground) -> jump, space (mid-air) -> double jump, space (back on ground) -> jump
	// So it's push -> pop
	_action_callback_mapping[action_name].emplace_back(callback);

}

void InputManager::removeActionCallback(const std::string& action_name, const std::string& callback_reference) {

	// If there exists an action in the map, which reference is the same as the passed
	// string, the ActionCallback is removed from the map
	// "Removes any instance of this callback reference from a vector of callbacks"
	std::erase_if(_action_callback_mapping[action_name], [callback_reference](const ActionCallback& callback) {
			return callback.callback_reference == callback_reference;
		});

}

// --- MAP AND UNMAP INPUT TO/FROM ACTIONS --- //

void InputManager::mapInputToAction(InputKey key, const InputAction& action) {

	// TODO: Check for duplicates
	_input_action_mapping[key].emplace_back(action);

}

void InputManager::unmapInputFromAction(InputKey key, const std::string& action) {

	std::erase_if(_input_action_mapping[key], [action](const InputAction& input_action) {
			return input_action.action_name == action;
		});

}

void InputManager::processInput() {

	std::vector<ActionEvent> events {};

	// Go through all registered devices
	for (auto& device : _devices) {
		
		// First, we want to get the new state of the device (call the GLFW function)
		auto new_state = device.stateFunction(device.index);

		// Then we compare the new state to the old state (for all members)
		// new_state is the state of the device as a whole (map), whereas key_state is that of a single member
		for (auto& key_state : new_state) {

			if (device.current_state[key_state.first].value != key_state.second.value) {
				
				// TO-DO: Fix cases where conflicting buttons are pressed -> LEFT AND RIGHT

				// Generate device action event if the values are different (state changed)
				auto generated_events = generateActionEvent(device.index, key_state.first, key_state.second.value);
				events.insert(events.end(), generated_events.begin(), generated_events.end());

				// Override the value after generating the action event (save new state)
				device.current_state[key_state.first].value = key_state.second.value;

			}

		}

	}

	// Propagate action events
	for (auto& event : events) {
	
		propagateActionEvent(event);

	}
	
}

std::vector<InputManager::ActionEvent> InputManager::generateActionEvent(int device_index, InputKey key, float new_value) {

	std::vector<ActionEvent> action_events {};

	auto& actions = _input_action_mapping[key];
	InputSource source = getInputSourceFromKey(key);

	for (auto& action : actions) {
		
		action_events.emplace_back(ActionEvent {

			.source = source,
			.source_index = device_index,
			.value = new_value * action.scale,
			.action_name = action.action_name

		});

	}

	return action_events;

}

void InputManager::propagateActionEvent(ActionEvent event) {

	// We go through the stack backwards and try to resolve the latest action event
	for (size_t i = _action_callback_mapping[event.action_name].size() - 1; i >= 0; i--) {
	
		auto& action = _action_callback_mapping[event.action_name][i];

		// If we manage to propagate an action event at some point in the stack,
		// we want to break the loop (resolve only the top one - first possible)
		if (action.function(event.source, event.source_index, event.value)) { break; }

	}

}

void InputManager::registerDevice(const InputDevice& device) {
	
	std::cout << "Registered device of type: " << static_cast<int>(device.type) << std::endl;

	// Add device to our list of devices
	_devices.emplace_back(device);

}

void InputManager::removeDevice(InputDeviceType type, int input_index) {

	// Removes any device with this type and index from the _devices list
	std::erase_if(_devices, [type, input_index](const InputDevice& device) {
			return device.type == type && device.index == input_index;
		});

}

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
	case InputKey::MOUSE_POSX:
	case InputKey::MOUSE_POSY:
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
