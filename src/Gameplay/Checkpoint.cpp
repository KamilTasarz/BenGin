#include "Checkpoint.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "../Basic/Animator.h"
#include "GameManager.h"
#include "RewindManager.h"
#include "Puzzles/Electrified.h"
#include "../Component/BoundingBox.h"

REGISTER_SCRIPT(Checkpoint);

void Checkpoint::onAttach(Node* owner)
{
	this->owner = owner;
}

void Checkpoint::onDetach()
{
	owner = nullptr;
}

void Checkpoint::onStart()
{
	minColliderPosition = owner->AABB_logic->min_point_world;
	maxColliderPosition = owner->AABB_logic->max_point_world;
}

void Checkpoint::onUpdate(float deltaTime)
{
	if (owner->is_animating && !owner->animator->isPlayingNonLooping()) return;
	
	Node* player = GameManager::instance().currentPlayer;

	if (!owner || !player) return;

	owner->AABB_logic->min_point_world = minColliderPosition;
	owner->AABB_logic->max_point_world = maxColliderPosition;

	//rotate towards player
	glm::vec3 playerPos = player->transform.getGlobalPosition();
	glm::vec3 ownerPos = owner->transform.getGlobalPosition();

	// Patrzenie tylko w poziomie (ignoruj Y)
	glm::vec3 flatDirection = glm::normalize(glm::vec3(
		playerPos.x - ownerPos.x,
		0.0f,
		playerPos.z - ownerPos.z
	));

	glm::quat lookRotation = glm::quatLookAt(flatDirection, glm::vec3(0.f, 1.f, 0.f));
	glm::quat extraRotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.f, 1.f, 0.f));

	glm::quat finalRotation = lookRotation * extraRotation;

	owner->transform.setLocalRotation(finalRotation);

}

void Checkpoint::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		Node* playerSpawner = owner->scene_graph->root->getChildByTag("PlayerSpawner");
		playerSpawner->transform.setLocalPosition(owner->transform.getGlobalPosition() * glm::vec3(1.f, 1.f, 1.f) + glm::vec3(-2.f, 0.05f, 2.5f));
		//owner->animator->playAnimation(owner->pModel->animations[0], false);
		if (!owner->is_animating) {
			owner->animator->playAnimation(owner->pModel->getAnimationByName("ArmatureAction.001"), false);
			owner->is_animating = true;
			owner->animator->current_animation->speed = 1500.f;

			//RewindManager::Instance().resetAllHistories();
		}
	}
}

void Checkpoint::onStayCollisionLogic(Node* other)
{
	auto* electrified = other->getComponent<Electrified>();
	if (electrified && electrified->isActive) {
		GameManager::instance().RemoveThisPlayer(other);
	}
}
