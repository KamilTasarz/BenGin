#include "PowerActivator.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "Electrified.h"
#include "Door.h"
#include "Fan.h"

REGISTER_SCRIPT(PowerActivator);

void PowerActivator::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "PowerActivator::onAttach::" << owner->name << std::endl;
	//object = nullptr;
	//activate = false;
	//isActivated = false;
}

void PowerActivator::onDetach()
{
	std::cout << "PowerActivator::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void PowerActivator::onStart()
{
	std::cout << "PowerActivator::onStart::" << owner->name << std::endl;
	//object = nullptr;
	//activate = false;
	isActivated = false;
}

void PowerActivator::onUpdate(float deltaTime)
{
	if (isActivated) {
		//std::cout << "PowerActivator::onUpdate::" << owner->name << std::endl;
	}
	else {
		//std::cout << "PowerActivator not activated" << std::endl;
	}
}

void PowerActivator::onStayCollisionLogic(Node* other)
{
	Electrified* electrified = other->getComponent<Electrified>();
	if (electrified == nullptr) return;

	if (!electrified->isActive) return;

	//std::cout << "PowerActivator::onCollisionLogic::" << owner->name << std::endl;
	isActivated = true;
	ChangeState(activate);
}

void PowerActivator::onExitCollisionLogic(Node* other)
{
	Electrified* electrified = other->getComponent<Electrified>();
	if (electrified == nullptr) return;

	//std::cout << "PowerActivator::onCollisionLogic::" << owner->name << std::endl;
	isActivated = false;
	ChangeState(!activate);
}

void PowerActivator::ChangeState(bool state)
{
	if (object->getComponent<Door>() != nullptr) {
		object->getComponent<Door>()->ChangeState(state);
	}
	else if (object->getComponent<Fan>() != nullptr) {
		object->getComponent<Fan>()->isActive = state;
	}
}
