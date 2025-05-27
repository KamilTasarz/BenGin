#include "Door.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"

REGISTER_SCRIPT(Door);

void Door::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "Door::onAttach::" << owner->name << std::endl;
	//owner->setActive(!isOpen);
}

void Door::onDetach()
{
	std::cout << "Door::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Door::onStart()
{
	std::cout << "Door::onStart::" << owner->name << std::endl;
	//owner->setActive(!isOpen);
}

void Door::onUpdate(float deltaTime)
{
	//std::cout << "Door::onUpdate::" << owner->name << std::endl;

	//owner->setActive(!isOpen);
}

void Door::ChangeState(bool state)
{
	owner->setActive(state);
	//isOpen = !state;
}




