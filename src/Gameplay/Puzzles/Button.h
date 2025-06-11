#pragma once

#include "../Script.h"

class Button : public Script
{
public:
	using SelfType = Button;

	VARIABLE(Node*, object);
	VARIABLE(bool, activate);
	VARIABLE(Node*, secondObject);
	VARIABLE(bool, activateSecond)
	VARIABLE(bool, isPressed);

	int pressingObjects = 0;
	glm::vec3 startPos;
	glm::vec3 targetPos;
	Node* button = nullptr;
	Node* body = nullptr;

	Button() = default;
	virtual ~Button() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void ChangeState(bool state, Node* object);
	void onCollisionLogic(Node* other);
	void onStayCollisionLogic(Node* other) override;
	void onExitCollisionLogic(Node* other) override;
	//void onEnd() override;

	std::vector<Variable*> getFields() const override {
		static Variable objectVar = getField_object();
		static Variable activateVar = getField_activate();
		static Variable secondObjectVar = getField_secondObject();
		static Variable activateSecondVar = getField_activateSecond();
		static Variable isPressedVar = getField_isPressed();

		return { &objectVar, &activateVar, &secondObjectVar, &activateSecondVar, &isPressedVar };
	}
};

