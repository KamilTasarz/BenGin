#include "Fan.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "../System/Rigidbody.h"

REGISTER_SCRIPT(Fan);

void Fan::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "Fan::onAttach::" << owner->name << std::endl;
}

void Fan::onDetach()
{
	std::cout << "Fan::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Fan::onStart()
{
	// Initialize the fan state
	// isActive = false;
}

void Fan::onUpdate(float deltaTime)
{
	// Update the fan state
	if (isActive) {
		//std::cout << "Fan is active" << std::endl;
	}
	else {
		//std::cout << "Fan is inactive" << std::endl;
	}
}

void Fan::onStayCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player" || other->getTagName() == "Box") {
		//std::cout << "Fan is active - " << owner->name << std::endl;
		Rigidbody* rb = other->getComponent<Rigidbody>();
		if (rb) {
			rb->overrideVelocityY = true;
			rb->velocityX += horizontalPower;
			rb->velocityY += verticalPower;
		}
	}
}


