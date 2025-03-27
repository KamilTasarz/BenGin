#include "InputManager.h"

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
