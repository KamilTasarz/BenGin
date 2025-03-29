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

void InputManager::mapInputToAction(InputKey key, const std::string& action) {

	_input_action_mapping[key].emplace(action);

}

void InputManager::unmapInputFromAction(InputKey key, const std::string& action) {

	// Because it's a set it will only be present once (no copies of the same action for one key)
	_input_action_mapping[key].erase(action);

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
				
				// Generate device action event if the values are different (state changed)
				events.emplace_back(generateActionEvent(device.index, key_state.first,
					device.current_state[key_state.first].value, key_state.second.value));

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

InputManager::ActionEvent InputManager::generateActionEvent(int device_index, InputKey key, float old_value, float new_value) {

	return ActionEvent();

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
