#pragma once

#include <glm/glm.hpp>
#include "Component.h"

class Rigidbody : public Component
{
public:
	float velocityX, velocityY;;
	float targetVelocityX, targetVelocityY;

	bool overrideVelocityX = false, overrideVelocityY = false;

	float velocityDeltaX, velocityDeltaY;

	float smoothingFactor = 10.0f;
	float drag = 0.f;
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
	glm::vec4 side = glm::vec4(0.f, 0.f, 1.f, 0.f);
	glm::vec4 up = glm::vec4(0.f, 1.f, 0.f, 0.f);

	bool isPlayer = false;

	Rigidbody(float mass = 1.f, float gravity = 1.f, bool isStatic = false, bool useGravity = true, bool lockPositionX = false, bool lockPositionY = false, bool lockPositionZ = false);
	virtual ~Rigidbody() = default;
	void onAttach(Node* node) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onCollision(Node* other) override;
	void onStayCollision(Node* other) override;
	void onExitCollision(Node* other) override;
};

