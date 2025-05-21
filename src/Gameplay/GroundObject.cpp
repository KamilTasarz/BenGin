#include "GroundObject.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "PlayerController.h"
#include "../System/Rigidbody.h"

REGISTER_SCRIPT(GroundObject);

void GroundObject::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "GroundObject::onAttach::" << owner->name << std::endl;
}

void GroundObject::onDetach()
{
	std::cout << "GroundObject::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void GroundObject::onUpdate(float deltaTime)
{
}

void GroundObject::onStayCollision(Node* other)
{
	/*if (other->getTagName() == "Player" || other->getTagName() == "Box") {
		Rigidbody* rb = other->getComponent<Rigidbody>();
		
		if (!rb->groundUnderneath) return;

		rb->isGrounded = true;
		rb->timer = 0.0f;
		justGrounded = true;
	}*/
}

void GroundObject::onExitCollision(Node* other)
{
	/*if (other->getTagName() == "Player" || other->getTagName() == "Box") {
		Rigidbody* rb = other->getComponent<Rigidbody>();
		rb->isGrounded = false;
		rb->velocityYResetted = false;
		rb->timer = 0.1f;
	}*/
}
