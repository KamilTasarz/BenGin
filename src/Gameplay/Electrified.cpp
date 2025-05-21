#include "Electrified.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "PlayerController.h"

REGISTER_SCRIPT(Electrified);

void Electrified::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "Electrified::onAttach::" << owner->name << std::endl;
}

void Electrified::onDetach()
{
	std::cout << "Electrified::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Electrified::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		other->addComponent(std::make_unique<Electrified>());
		other->getComponent<PlayerController>()->Die(false, true);
	}
}
