#include "GroundObject.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "PlayerController.h"
#include "../System/Rigidbody.h"

REGISTER_SCRIPT(GroundObject);

void GroundObject::onAttach(Node* owner)
{
	this->owner = owner;
	//std::cout << "GroundObject::onAttach::" << owner->name << std::endl;
}

void GroundObject::onDetach()
{
	//std::cout << "GroundObject::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void GroundObject::onUpdate(float deltaTime)
{
}

void GroundObject::onStayCollision(Node* other)
{

}

void GroundObject::onExitCollision(Node* other)
{

}
