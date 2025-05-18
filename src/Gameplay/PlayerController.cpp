#include "PlayerController.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "../System/Rigidbody.h"
#include "../System/PhysicsSystem.h"
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
	std::cout << PrefabRegistry::Get()[0]->prefab_scene_graph->root->name << std::endl;
}
void PlayerController::onUpdate(float deltaTime)
{
	glm::vec3 position = owner->transform.getLocalPosition();
	Rigidbody* rb = owner->getComponent<Rigidbody>();

	if (!rb) return;

	rb->targetVelocityX = ((glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_RIGHT) == GLFW_PRESS) - (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_LEFT) == GLFW_PRESS)) * speed;


	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		std::cout << "Gracz probuje skoczyc" << std::endl;

		if (isGrounded) {
			{
				rb->velocityY = jumpForce;
				isGrounded = false;
				isJumping = true;
			}
		}
	}

	Ray ray;
	ray.direction = glm::vec4(1.f, 0.f, 0.f, 1.f);
	ray.origin = owner->transform.getGlobalPosition();
	std::vector<Node*> nodes;
	if (PhysicsSystem::instance().rayCast(ray, nodes, 4.f)) {
		if (!(nodes.size() == 1 && nodes[0] == owner))
			std::cout << nodes.size() << std::endl;
		
	}
	else {
		std::cout << "brak" << std::endl;
	}

	//if (doors) std::cout << "doors::" << doors->name << std::endl;
	//if (speed > 6.f) std::cout << "speed::" << speed << std::endl;
}


void PlayerController::onEnd()
{
}

void PlayerController::onCollision(Node* other)
{
}

void PlayerController::onStayCollision(Node* other)
{
	//std::cout << "Gracz koliduje z podloga" << std::endl;

	if (other->getLayerName() == "Floor") {
		isGrounded = true;
		velocityY = 0.f;
	}
	else {
		isGrounded = false;
	}

	if (other->getTagName() == "Wall") {
		owner->transform.setLocalPosition(owner->transform.getLocalPosition() + glm::vec3(0.f, 0.5f, 0.f));
	}
}

void PlayerController::onExitCollision(Node* other)
{
	//std::cout << "PlayerController::onExitCollision::" << owner->name << std::endl;

	if (other->getLayerName() == "Floor") {
		isGrounded = false;
	}
}

void PlayerController::Jump()
{
	velocityY = jumpForce;
	isGrounded = false;
	isJumping = true;
}
