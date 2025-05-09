#include "PlayerController.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"

REGISTER_SCRIPT(PlayerController);

void PlayerController::onAttach(Node* owner)
{
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
}


void PlayerController::onEnd()
{
}

void PlayerController::onCollision(Node* other)
{
	std::cout << "PlayerController::onCollision::" << owner->name << std::endl;
	std::cout << "PlayerController::onCollision(other)::" << other->name << std::endl;
}
