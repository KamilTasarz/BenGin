#include "PlayerController.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"

REGISTER_SCRIPT(PlayerController);

void PlayerController::onAttach(Node* owner)
{
	//speed = 5.f;
	//doors = nullptr;
	this->owner = owner;
	std::cout << "PlayerController::onAttach::" << owner->name << std::endl;
}
void PlayerController::onDetach()
{
	std::cout << "PlayerController::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}
void PlayerController::onStart()
{
}
void PlayerController::onUpdate(float deltaTime)
{
	glm::vec3 position = owner->transform.getLocalPosition();

	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		position.x -= 0.1f;
	}

	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		position.x += 0.1f;
	}

	owner->transform.setLocalPosition(position);

	//if (doors) std::cout << "doors::" << doors->name << std::endl;
	//if (speed > 6.f) std::cout << "speed::" << speed << std::endl;

}


void PlayerController::onEnd()
{
}

void PlayerController::onCollision(Node* other)
{
	std::cout << "PlayerController::onEnterCollision::" << owner->name << std::endl;
}

void PlayerController::onStayCollision(Node* other)
{
	std::cout << "PlayerController::onStayCollision::" << owner->name << std::endl;
	std::cout << "PlayerController::onStayCollision(other)::" << other->name << std::endl;

	if (other->getTagName() == "Wall") {
		owner->transform.setLocalPosition(owner->transform.getLocalPosition() + glm::vec3(0.f, 0.5f, 0.f));
	}
}

void PlayerController::onExitCollision(Node* other)
{
	std::cout << "PlayerController::onExitCollision::" << owner->name << std::endl;
}

void PlayerController::onCollisionLogic(Node* other)
{
	std::cout << "logiczna" << other->name << std::endl;
}

void PlayerController::onStayCollisionLogic(Node* other)
{
	std::cout << "logiczna stay" << other->name << std::endl;
}

void PlayerController::onExitCollisionLogic(Node* other)
{
	std::cout << "logiczna exit" << other->name << std::endl;
}
