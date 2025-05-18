#include "Spikes.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "PlayerController.h"

REGISTER_SCRIPT(Spikes);

void Spikes::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "Spikes::onAttach::" << owner->name << std::endl;
}

void Spikes::onDetach()
{
	std::cout << "Spikes::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Spikes::onCollision(Node* other)
{
	if (other->getTagName() == "Player") {
		other->getComponent<PlayerController>()->Die(true);
	}
}
