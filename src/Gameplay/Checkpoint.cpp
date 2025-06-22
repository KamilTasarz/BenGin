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
	//minColliderPosition = owner->AABB_logic->min_point_world;
	//maxColliderPosition = owner->AABB_logic->max_point_world;

	arm = owner->getChildByNamePart("arm");
	tank = owner->getChildByNamePart("tank");
	filling = owner->getChildByNamePart("filling");

	if (tank) tank->changeColor(glm::vec4(0.25f, 0.6f, 0.25f, 0.6f));
	if (filling) filling->changeColor(glm::vec4(0.75f, 0.12f, 0.12f, 1.f));

	fillingStarted = false;
}

void Checkpoint::onUpdate(float deltaTime)
{
	if (!arm || !tank || !filling) return;
	
	if (fillingStarted) {
		if (timer < 0.f) {
			glm::vec3 currentScale = filling->transform.getLocalScale();
			glm::vec3 targetScale = glm::vec3(.75f, .6f, .75f);

			glm::vec3 newScale = glm::mix(currentScale, targetScale, deltaTime * 2.f);
			filling->transform.setLocalScale(newScale);

			glm::vec3 newTankScale = glm::vec3(.75f) - newScale;
			tank->transform.setLocalScale(glm::vec3(.75f, newTankScale.y, .75f));

			if (targetScale.y - newScale.y < 0.02f) {
				fillingStarted = false;
			}
		}
		else {
			timer -= deltaTime;
		}
	}

	if (arm->is_animating && !arm->animator->isPlayingNonLooping()) return;
	
	Node* player = GameManager::instance().currentPlayer;

	if (!owner || !player) return;

	glm::vec3 playerPos = player->transform.getGlobalPosition();
	glm::vec3 ownerPos = arm->transform.getGlobalPosition();

	glm::vec3 flatDirection = glm::normalize(glm::vec3(
		playerPos.x - ownerPos.x,
		0.0f,
		playerPos.z - ownerPos.z
	));

	glm::quat lookRotation = glm::quatLookAt(flatDirection, glm::vec3(0.f, 1.f, 0.f));
	glm::quat extraRotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.f, 1.f, 0.f));

	glm::quat finalRotation = lookRotation * extraRotation;

	arm->transform.setLocalRotation(finalRotation);

}

void Checkpoint::onCollisionLogic(Node* other)
{
	if (!arm || !tank || !filling) return;

	if (other->getTagName() == "Player") {
		Node* playerSpawner = owner->scene_graph->root->getChildByTag("PlayerSpawner");
		playerSpawner->transform.setLocalPosition(owner->transform.getGlobalPosition());
		if (!arm->is_animating) {
			arm->animator->playAnimation(arm->pModel->getAnimationByName("ArmatureAction.001"), false);
			arm->is_animating = true;
			arm->animator->current_animation->speed = 1500.f;

			timer = 0.5f;
			fillingStarted = true;
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
