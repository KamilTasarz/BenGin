#pragma once

#include "Script.h"
#include "../System/Rigidbody.h"

class Scale : public Script
{
public:
	using SelfType = Scale;

	VARIABLE(Node*, secondScale);

	Rigidbody* rb;
	glm::vec3 startPos1;
	glm::vec3 startPos2;
	bool isPlayerOn = false;
	float timer = 0.f;


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
		return { &secondScaleVar };
	}
};

