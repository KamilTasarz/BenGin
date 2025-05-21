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

	float mass;
	float gravity;
	bool is_static;
	bool useGravity;
	bool isGrounded;
	bool lockPositionX, lockPositionY, lockPositionZ;
	glm::vec3 startPos;

	bool velocityYResetted = false;
	float timer = 0.f;
	bool groundUnderneath = false;
	bool ceilingAbove = false;
	

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

