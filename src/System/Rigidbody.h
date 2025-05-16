#pragma once

#include <glm/glm.hpp>
#include "Component.h"

class Rigidbody : public Component
{
public:
	float velocityX;
	float targetVelocityX;
	float velocityY;
	float targetVelocityY;
	bool overrideVelocityY = false;

	float mass;
	float gravity;
	bool is_static;
	bool useGravity;
	bool lockPositionX;
	bool lockPositionY;
	bool lockPositionZ;
	glm::vec3 startPos;
	

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

