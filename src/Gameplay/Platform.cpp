#include "Platform.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "../System/Rigidbody.h"

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
		timer = 0.25f;
		//std::cout << "spadanie z platformy, timer: " << timer << std::endl;
	}
	
	if (timer > 0.f) {
		timer -= deltaTime;
		if (timer <= 0.f) {
			owner->is_physic_active = true;
			timer = 0.f;
		}
	}
	else {
		owner->is_physic_active = true;
	}
}

void Platform::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		owner->is_physic_active = false;
		timer = 0.25f;

		//owner->setActive(false);
		std::cout << "wejscie pod platforme" << std::endl;
	}
}

void Platform::onExitCollisionLogic(Node* other)
{
	//if (other->getTagName() == "Player") {
	//	//owner->is_physic_active = true;

	//	//owner->setActive(true);
	//	std::cout << "wyjscie z platformy" << std::endl;
	//}
}

void Platform::onStayCollision(Node* other)
{
	if (other->getTagName() == "Player") {
		//other->getComponent<Rigidbody>()->overrideVelocityY = true;
	}
}
