#include "Platform.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "../System/Rigidbody.h"
#include "../Component/BoundingBox.h"
#include "../System/Tag.h"
#include "GameManager.h"
#include "PlayerController.h"

REGISTER_SCRIPT(Platform);

void Platform::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "Platform::onAttach::" << owner->name << std::endl;
	owner->AABB->addIgnoredLayer(TagLayerManager::Instance().getLayer("NPC"));
}

void Platform::onDetach()
{
	std::cout << "Platform::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Platform::onUpdate(float deltaTime)
{
	bool isFlippedAndPressedUp = flipped && (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_UP) == GLFW_PRESS);

	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_DOWN) == GLFW_PRESS || isFlippedAndPressedUp) {
		//owner->setPhysic(false);
		owner->AABB->addIgnoredLayer(TagLayerManager::Instance().getLayer("Player"));
		timer = 0.25f;
	}
	
	if (timer > 0.f) {
		timer -= deltaTime;
		if (timer <= 0.f) {
			owner->setPhysic(true);
			timer = 0.f;
		}
	}
	else {
		//owner->setPhysic(true);
		owner->AABB->removeIgnoredLayer(TagLayerManager::Instance().getLayer("Player"));
	}

	auto* player = GameManager::instance().currentPlayer;
	if (!player) return;

	bool isGravityFlipped = player->getComponent<PlayerController>()->isGravityFlipped;

	if (!flipped && isGravityFlipped) {
		owner->transform.setLocalRotation(glm::angleAxis(glm::radians(180.0f), glm::vec3(0, 0, 1)));
		flipped = true;
	}
	else if (flipped && !isGravityFlipped) {
		owner->transform.setLocalRotation(glm::quat(1, 0, 0, 0));
		flipped = false;
	}
}

void Platform::onStayCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		//owner->setPhysic(false);
		owner->AABB->addIgnoredLayer(TagLayerManager::Instance().getLayer("Player"));
		timer = 0.25f;

		//owner->setActive(false);
		std::cout << "wejscie pod platforme" << std::endl;
	}
}

void Platform::onExitCollisionLogic(Node* other)
{
	//if (other->getTagName() == "Player") {
	//	//owner->setPhysic(true);

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
