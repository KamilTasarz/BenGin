#include "Virus.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "../PlayerController.h"
#include "../../System/Rigidbody.h"

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
	/*glm::vec3 position = owner->transform.getLocalPosition();
	glm::quat rotation = owner->transform.getLocalRotation();

	float rotationSpeed = 1.f;
	float rotationAngle = rotationSpeed * deltaTime;

	rotation = glm::rotate(rotation, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	
	owner->transform.setLocalRotation(rotation);*/
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
	PlayerController* player = target->getComponent<PlayerController>();

	target->getComponent<Rigidbody>()->gravity = -32.f;
	target->getComponent<Rigidbody>()->mass = 1.f;
	player->speed = 9.f;
	player->isGravityFlipped = false;
	player->jumpForce = 19.f;
	player->virusType = "none";

	VirusEffect(target);

	std::cout << "Virus effect applied to: " << target->name << std::endl;
}

void Virus::VirusEffect(Node* target)
{
	PlayerController* player = target->getComponent<PlayerController>();

	if (blue) {
		target->changeColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		player->isGravityFlipped = false;
		target->getComponent<Rigidbody>()->gravity = -32.f;
		player->virusType = "blue";
	}
	else if (green) {
		target->changeColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

		player->isGravityFlipped = true;
		target->getComponent<Rigidbody>()->gravity = 32.f;
		player->virusType = "green";
	}
	else if (black) {
		target->changeColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

		target->getComponent<Rigidbody>()->mass = 15.f;
		player->speed *= 0.7f;
		player->jumpForce *= 0.8f;
		player->virusType = "black";
	}
	else {
		std::cout << "Unknown virus type!" << std::endl;
	}
}


