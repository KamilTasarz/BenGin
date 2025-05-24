#pragma once

#include "Script.h"
#include "../System/Rigidbody.h"
#include "../Basic/Animation.h"

class IPlayerAnimState;

class PlayerAnimationController : public Script
{
public:
	Rigidbody* rb;

	Animation* idle;
	Animation* turn;
	Animation* run;
	Animation* sleep;
	Animation* jump;
	Animation* inAir;
	Animation* fall;

	glm::vec3 previousPosition;
	float deltaX, deltaY;

	IPlayerAnimState* currentState = nullptr;

	bool gravityFlipped = false;
	bool facingRight = true;

	PlayerAnimationController() = default;
	virtual ~PlayerAnimationController() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onEnd() override;
	void changeState(IPlayerAnimState* newState);
};

