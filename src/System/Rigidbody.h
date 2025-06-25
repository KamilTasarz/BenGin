#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Component.h"
#include <deque>

struct RigidBodySnapshot {
	glm::vec3 position;
	glm::quat rotation;
};

class Rigidbody : public Component
{
public:
	float velocityX, velocityY, velocityZ;
	float velocityXBuffer, velocityYBuffer;
	float targetVelocityX, targetVelocityY;

	bool isBufferX = false, isBufferY = false;

	bool overrideVelocityX = false, overrideVelocityY = false;
	bool useVelocityZ = false;

	float velocityDeltaX, velocityDeltaY;

	float smoothingFactor = 10.0f;
	float drag = 0.f, dragY = 0.f;
	float mass;
	float gravity;
	bool is_static;
	bool useGravity;
	bool isGrounded;
	bool lockPositionX, lockPositionY, lockPositionZ;
	glm::vec3 startPos;

	bool velocityYResetted = false;
	float timer = 0.f;
	bool groundUnderneath = false, scaleUnderneath = false;
	bool ceilingAbove = false;
	bool isPushing = false;
	
	bool isGravityFlipped = false;
	float length;
	float width;
	glm::vec4 side = glm::vec4(1.f, 0.f, 0.f, 0.f);

	bool isPlayer = false;

	//time reverse
	std::deque<glm::vec3> positionHistory;
	std::deque<glm::quat> rotationHistory;
	bool isRewinding = false;

	Rigidbody(float mass = 1.f, float gravity = 1.f, bool isStatic = false, bool useGravity = true, bool lockPositionX = false, bool lockPositionY = false, bool lockPositionZ = false);
	virtual ~Rigidbody() = default;
	void onAttach(Node* node) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onCollision(Node* other) override;
	void onStayCollision(Node* other) override;
	void onExitCollision(Node* other) override;
	bool tryingToMoveAwayFrom(Node* other);
	bool isMovingTowards(Node* other, float axis);
};

