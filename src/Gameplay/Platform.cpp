#include "Platform.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"

REGISTER_SCRIPT(Platform);

void Platform::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "Platform::onAttach::" << owner->name << std::endl;
}

void Platform::onDetach()
{
	std::cout << "Platform::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Platform::onUpdate(float deltaTime)
{
	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		owner->is_physic_active = false;
	}
	else {
		owner->is_physic_active = true;
	}

	//owner->is_physic_active = true;
}

void Platform::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		owner->is_physic_active = false;

		//owner->setActive(false);
		std::cout << "wejscie pod platforme" << std::endl;
	}
}

void Platform::onExitCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		owner->is_physic_active = true;

		//owner->setActive(true);
		std::cout << "wyjscie z platformy" << std::endl;
	}
}

void Platform::onExitCollision(Node* other)
{
	/*if (other->getTagName() == "Player") {
		owner->is_physic_active = true;
	}*/
}
