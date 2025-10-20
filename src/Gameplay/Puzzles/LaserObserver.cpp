#include "LaserObserver.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "Electrified.h"
#include "Door.h"
#include "Fan.h"

REGISTER_SCRIPT(LaserObserver);

void LaserObserver::onAttach(Node* owner)
{
	this->owner = owner;
	//std::cout << "PowerActivator::onAttach::" << owner->name << std::endl;
	//object = nullptr;
	//activate = false;
	//isActivated = false;
}

void LaserObserver::onDetach()
{
	//std::cout << "PowerActivator::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void LaserObserver::onStart()
{
	//std::cout << "PowerActivator::onStart::" << owner->name << std::endl;
	//object = nullptr;
	//activate = false;
	isActivated = false;
}

void LaserObserver::onUpdate(float deltaTime)
{
	if (isActivated && !activated) {
		ChangeState(activate);
		activated = true;
	}
	else if (deactivationTimer < 0.f && activated) {
		ChangeState(!activate);
		activated = false;
		isActivated = false;
	}
	else {
		deactivationTimer -= deltaTime;
	}
}

void LaserObserver::ChangeState(bool state)
{
	if (object->getComponent<Door>() != nullptr) {
		object->getComponent<Door>()->ChangeState(state);
	}
	else if (object->getComponent<Fan>() != nullptr) {
		object->getComponent<Fan>()->isActive = state;
	}
}

void LaserObserver::Activate()
{
	isActivated = true;
	deactivationTimer = 0.1f;
}
