#include "Virus.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"

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
	std::cout << "Ser podniesiony - " << owner->name << std::endl;
	ApplyEffect(other);
}

void Virus::ApplyEffect(Node* target)
{
	// Implement the effect logic here
	VirusEffect(target);

	std::cout << "Virus effect applied to: " << target->name << std::endl;
}

void Virus::VirusEffect(Node* target)
{
	if (type == virusType::BLUE) {
		// Apply blue virus effect
		target->changeColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	}
	else if (type == virusType::GREEN) {
		// Apply green virus effect
		target->changeColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}
	else if (type == virusType::BLACK) {
		// Apply black virus effect
		target->changeColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}
	else {
		std::cout << "Unknown virus type!" << std::endl;
	}
}


