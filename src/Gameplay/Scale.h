#pragma once

#include "Script.h"
#include "../System/Rigidbody.h"

class Scale : public Script
{
public:
	using SelfType = Scale;

	VARIABLE(Node*, secondScale);
	//VARIABLE(Node*, leftPin);
	//VARIABLE(Node*, rightPin);
	//VARIABLE(bool, moveHorizontally);
	//VARIABLE(bool, returnToPosition);

	Rigidbody* rb;
	glm::vec3 startPos1;
	glm::vec3 startPos2;
	bool isPlayerOn = false;
	float timer = 0.f;
	bool moveHorizontally = false;
	bool returnToPosition = true;


	Scale() = default;
	virtual ~Scale() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onEnd() override;
	void onStayCollision(Node* other) override;
	void onExitCollision(Node* other) override;
	//void onExitCollisionLogic(Node* other) override;

	std::vector<Variable*> getFields() const override {
		static Variable secondScaleVar = getField_secondScale();
		//static Variable leftPinVar = getField_leftPin();
		//static Variable rightPinVar = getField_rightPin();
		//static Variable moveHorizontallyVar = getField_moveHorizontally();
		//static Variable returnToPositionVar = getField_returnToPosition();
		return { &secondScaleVar, /*&leftPinVar, &rightPinVar , &moveHorizontallyVar, &returnToPositionVar */ };
	}
};

