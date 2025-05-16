#include "Virus.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "PlayerController.h"
#include "../System/Rigidbody.h"

REGISTER_SCRIPT(Virus);

void Virus::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "Virus::onAttach::" << owner->name << std::endl;
}

void Virus::onDetach()
{
	std::cout << "Virus::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Virus::onStart()
{
}

void Virus::onUpdate(float deltaTime)
{
	glm::vec3 position = owner->transform.getLocalPosition();
	glm::quat rotation = owner->transform.getLocalRotation();

	float rotationSpeed = 1.f;
	float rotationAngle = rotationSpeed * deltaTime;

	rotation = glm::rotate(rotation, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	
	owner->transform.setLocalRotation(rotation);
}

void Virus::onEnd()
{
}

void Virus::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		std::cout << "Ser podniesiony - " << owner->name << std::endl;
		ApplyEffect(other);
	}
}

void Virus::ApplyEffect(Node* target)
{
	// Implement the effect logic here
	VirusEffect(target);

	std::cout << "Virus effect applied to: " << target->name << std::endl;
}

void Virus::VirusEffect(Node* target)
{
	PlayerController* player = target->getComponent<PlayerController>();

	if (/*type == "blue"*/ blue) {
		// Apply blue virus effect
		target->changeColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		player->isGravityFlipped = false;
		target->getComponent<Rigidbody>()->gravity = -25.f;
	}
	else if (/*type == "green"*/ green) {
		// Apply green virus effect
		target->changeColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

		player->isGravityFlipped = true;
		target->getComponent<Rigidbody>()->gravity = 25.f;
	}
	else if (/*type == "black"*/ black) {
		// Apply black virus effect
		target->changeColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}
	else {
		std::cout << "Unknown virus type!" << std::endl;
	}
}


