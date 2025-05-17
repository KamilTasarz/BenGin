#include "PlayerController.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "../System/Rigidbody.h"
//#include "GameMath.h"

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
	isGravityFlipped = false;
}

void PlayerController::onUpdate(float deltaTime)
{
	glm::vec3 position = owner->transform.getLocalPosition();
	Rigidbody* rb = owner->getComponent<Rigidbody>();

	if (!rb) return;

	bool pressedRight = (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_D) == GLFW_PRESS);
	bool pressedLeft = (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_A) == GLFW_PRESS);

	rb->targetVelocityX = (pressedRight - pressedLeft) * speed;


	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		//std::cout << "Gracz probuje skoczyc" << std::endl;

		if (rb->isGrounded) {
			{
				//isGravityFlipped ? rb->velocityY = -jumpForce : jumpForce;
				if (isGravityFlipped) rb->velocityY = -jumpForce;
				else rb->velocityY = jumpForce;

				//rb->isGrounded = false;
				isJumping = true;
			}
		}
	}

	//if (doors) std::cout << "doors::" << doors->name << std::endl;
	//if (speed > 6.f) std::cout << "speed::" << speed << std::endl;
}


void PlayerController::onEnd()
{
}
