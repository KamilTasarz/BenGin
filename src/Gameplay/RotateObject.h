#pragma once

#include "Script.h"

class RotateObject : public Script
{
public:
	using SelfType = RotateObject;

	VARIABLE(float, rotationSpeed);
	VARIABLE(bool, rotateX);
	VARIABLE(bool, rotateY);
	VARIABLE(bool, rotateZ);
	VARIABLE(bool, stopRotation);

	RotateObject() = default;
	virtual ~RotateObject() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onEnd() override;

	std::vector<Variable*> getFields() const override {
		static Variable rotationSpeedVar = getField_rotationSpeed();
		static Variable rotateXVar = getField_rotateX();
		static Variable rotateYVar = getField_rotateY();
		static Variable rotateZVar = getField_rotateZ();
		static Variable stopRotationVar = getField_stopRotation();
		return { &rotationSpeedVar, &rotateXVar, &rotateYVar, &rotateZVar, &stopRotationVar };
	}
};

