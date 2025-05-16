#pragma once

#include "Script.h"

class RotateObject : public Script
{
public:
	using SelfType = RotateObject;

	VARIABLE(float, rotationSpeed);

	RotateObject() = default;
	virtual ~RotateObject() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onEnd() override;

	std::vector<Variable*> getFields() const override {
		static Variable rotationSpeedVar = getField_rotationSpeed();
		return { &rotationSpeedVar };
	}
};

