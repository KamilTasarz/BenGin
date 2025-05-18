#include "PlayerController.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "../System/Rigidbody.h"
#include "../System/Tag.h"
#include "PlayerSpawner.h"
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
	isGravityFlipped = false;
	//std::cout << PrefabRegistry::Get()[0]->prefab_scene_graph->root->name << std::endl;
}

void PlayerController::onUpdate(float deltaTime)
{
	if (isDead) return;
	
	std::cout << "tag aktualnego gracza" << owner->getTagName() << std::endl;

	glm::vec3 position = owner->transform.getLocalPosition();
	Rigidbody* rb = owner->getComponent<Rigidbody>();

	if (!rb) return;

	bool pressedRight = (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_D) == GLFW_PRESS);
	bool pressedLeft = (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_A) == GLFW_PRESS);

	rb->targetVelocityX = (pressedRight - pressedLeft) * speed;

	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_K) == GLFW_PRESS) {
		Die(false);
	}

	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		//std::cout << "Gracz probuje skoczyc" << std::endl;

		if (rb->isGrounded) {
			{
				rb->overrideVelocityY = true;
				if (isGravityFlipped) rb->velocityY = -jumpForce;
				else rb->velocityY = jumpForce;

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

void PlayerController::Die(bool freeze)
{
	if (isDead) return;
	
	Rigidbody* rb = owner->getComponent<Rigidbody>();
	rb->is_static = freeze;

	std::shared_ptr<Tag> tag = TagLayerManager::Instance().getTag("Box");
	owner->setTag(tag);
	isDead = true;

	// spawn new player
	Node* playerSpawner = owner->scene_graph->root->getChildByTag("PlayerSpawner");
	playerSpawner->getComponent<PlayerSpawner>()->spawnPlayer();
}
