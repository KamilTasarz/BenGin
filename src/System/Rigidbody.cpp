#include "Rigidbody.h"

#include "../Basic/Node.h"

Rigidbody::Rigidbody(float mass, float gravity, bool isStatic)
{
	this->mass = mass;
	this->gravity = gravity;
	this->is_static = isStatic;
	name = "Rigidbody";
}

void Rigidbody::onAttach(Node* node)
{
	owner = node;
}

void Rigidbody::onDetach()
{
	owner = nullptr;
}

void Rigidbody::onUpdate(float deltaTime)
{
	if (is_static) {
		return;
	}
	if (owner->transform.getLocalPosition().y > -10.f) {
		owner->transform.setLocalPosition(owner->transform.getLocalPosition() + glm::vec3(0.f, - mass * gravity * deltaTime, 0.f));
	}
	else {
		owner->transform.setLocalPosition(owner->transform.getLocalPosition() + glm::vec3(0.f, 0.f, 0.f));
	}
}

void Rigidbody::onCollision(Node* other)
{
}
