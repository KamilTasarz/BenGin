#pragma once

#include "Component.h"

class Rigidbody : public Component
{
public:
	float mass;
	float gravity;
	bool is_static;
	

	Rigidbody(float mass = 1.f, float gravity = 1.f, bool isStatic = true);
	virtual ~Rigidbody() = default;
	void onAttach(Node* node) override;
	void onDetach() override;
	void onUpdate(float deltaTime) override;
	void onCollision(Node* other) override;
};

