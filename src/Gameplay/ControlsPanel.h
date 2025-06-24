#pragma once

#include "Script.h"

class ControlsPanel final : public Script
{
public:
	using SelfType = ControlsPanel;

	VARIABLE(int, keyboardModelID);
	VARIABLE(int, ControllerModelID);

	bool modelChanged = false;
	bool gamepadInput = true;

	ControlsPanel() = default;
	~ControlsPanel() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;

	std::vector<Variable*> getFields() const override {
		static Variable keyboardModelIDVar = getField_keyboardModelID();
		static Variable ControllerModelIDVar = getField_ControllerModelID();
		return { &keyboardModelIDVar, &ControllerModelIDVar };
	}
};