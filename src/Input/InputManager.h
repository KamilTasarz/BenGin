#pragma once

#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <iostream>

#include <algorithm>
#include <functional>

#include "InputKey.h"
#include "InputDevice.h"

class InputManager {

public:

	using actionCallbackFunction = std::function<bool(InputSource, int, float)>;

	struct ActionCallback {

		std::string callback_reference;

		actionCallbackFunction function;

	};

	// Constructor and destructor
	InputManager();
	~InputManager();

	// We connect a specific action to its callback (which is a method along with a string to reference)
	void registerActionCallback(const std::string& action_name, const ActionCallback& callback);
	// If we want to remove the connection of an action with its callback from the map
	void removeActionCallback(const std::string& action_name, const std::string& callback_reference);

	// We associate performing an input (like pressing a certain key) with an action
	void mapInputToAction(InputKey key, const std::string& action);
	// We use it to make an input no longer be connected to an action
	void unmapInputFromAction(InputKey key, const std::string& action);

private:

	struct ActionEvent {

		InputSource source;
		int source_index;
		float value;
		std::string action_name;
	
	};

	// This function will compare the new state of the device with the old one and evaluate action events
	// This is our main input loop
	void processInput();

	ActionEvent generateActionEvent(int device_index, InputKey key, float old_value, float new_value);

	void propagateActionEvent(ActionEvent event);

	bool is_active { false };

	// By default both are initialized to empty ({} does that)

	// This maps the enum class "InputKey" to a specified actions defined by a set of strings
	// Which key press triggers which action
	// Set allows us to map a single key to multiple actions (different in GUI, different in game)
	std::unordered_map<InputKey, std::unordered_set<std::string>> _input_action_mapping {};

	// This maps actions (defined by a string) to a vector of "ActionCallback". Holds action callbacks
	// Holds what should happen when an action is triggered
	std::unordered_map<std::string, std::vector<ActionCallback>> _action_callback_mapping {};

	// List of registered devices
	std::vector<InputDevice> _devices {};

};

#endif // !INPUT_MANAGER_H
