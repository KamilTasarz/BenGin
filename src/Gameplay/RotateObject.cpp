#include "RotateObject.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"

REGISTER_SCRIPT(RotateObject);

void RotateObject::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "RotateObject::onAttach::" << owner->name << std::endl;
}	

void RotateObject::onDetach()
{
	std::cout << "RotateObject::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void RotateObject::onStart()
{
}

void RotateObject::onUpdate(float deltaTime)
{
	glm::quat rotation = owner->transform.getLocalRotation();

	float rotationAngle = rotationSpeed * deltaTime;
	rotation = glm::rotate(rotation, rotationAngle, glm::vec3(0.f, 1.f, 0.f));

	owner->transform.setLocalRotation(rotation);
}

void RotateObject::onEnd()
{
}


