#pragma once

#include "../Script.h"

class Button : public Script
{
public:
	using SelfType = Button;

	VARIABLE(Node*, object);
	VARIABLE(bool, activate)
	VARIABLE(bool, isPressed);
	glm::vec3 originalSize;

	Button() = default;
	virtual ~Button() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void ChangeState(bool state);
	void onStayCollisionLogic(Node* other) override;
	void onExitCollisionLogic(Node* other) override;
	//void onEnd() override;

	std::vector<Variable*> getFields() const override {
		static Variable objectVar = getField_object();
		static Variable activateVar = getField_activate();
		static Variable isPressedVar = getField_isPressed();

		return { &objectVar, &activateVar, &isPressedVar };
	}
};

