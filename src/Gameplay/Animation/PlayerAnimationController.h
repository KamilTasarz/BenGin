#pragma once

#include "../Script.h"
#include "../../System/Rigidbody.h"
#include "../PlayerController.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../AnimationRewindable.h"

class IPlayerAnimState;
class Animation;

class PlayerAnimationController : public Script
{
public:
	Rigidbody* rb;
	PlayerController* player;

	Animation* idle;
	Animation* turn;
	Animation* run;
	Animation* sleep;
	Animation* jump;
	Animation* rise;
	Animation* inAir;
	Animation* fall;
	Animation* land;
	Animation* push;
	Animation* deathLeft;
	Animation* deathRight;

	glm::vec3 previousPosition;
	float deltaX, deltaY;

	IPlayerAnimState* currentState = nullptr;

	bool gravityFlipped = false;
	bool facingRight = true;
	bool isRunning = false;
	bool isStanding = false;
	bool hasJumped = false;
	bool isRising = false;
	bool isFalling = false;
	bool hasLanded = false;

	bool isTurning = false;
	glm::quat targetRotation;
	float turnSpeed = 15.f;

	AnimationRewindable* rewindable = nullptr;

	PlayerAnimationController() = default;
	virtual ~PlayerAnimationController() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onEnd() override;
	void changeState(IPlayerAnimState* newState);
	void StartRotation(bool& conditionFlag, bool desiredState, float angleDegrees, const glm::vec3& axis);
	void UpdateRotation(float deltaTime);
};

