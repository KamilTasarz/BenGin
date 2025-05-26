#include "PlayerController.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "../System/Tag.h"
#include "PlayerSpawner.h"
#include "GroundObject.h"
#include "../Basic/Animator.h"
#include "../System/PhysicsSystem.h"
#include "../Component/BoundingBox.h"

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
 	rb = owner->getComponent<Rigidbody>();
	rb->lockPositionZ = true;
	rb->isPlayer = true;
	timerIndicator = owner->getChildById(0);
	scale_factor = owner->transform.getLocalScale().x;
}

void PlayerController::onUpdate(float deltaTime)
{
	if (isDead) return;

	//std::cout << "tag aktualnego gracza" << owner->getTagName() << std::endl;

	glm::vec3 position = owner->transform.getLocalPosition();

	if (!rb) return;

	if (rb->groundUnderneath || rb->scaleUnderneath) isJumping = false;

	if (isGravityFlipped && !rb->isGravityFlipped) {
		rb->isGravityFlipped = true;
	}
	else if (!isGravityFlipped && rb->isGravityFlipped) {
		rb->isGravityFlipped = false;
	}

	pressedRight = (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_D) == GLFW_PRESS);
	pressedLeft = (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_A) == GLFW_PRESS);

	if (!rb->overrideVelocityX) rb->targetVelocityX = (pressedRight - pressedLeft) * speed;

	if (pressedRight - pressedLeft != 0.f) isRunning = true;
	else isRunning = false;

	if (pressedRight) rb->side = glm::vec4(1.f, 0.f, 0.f, 0.f);
	else if (pressedLeft) rb->side = glm::vec4(-1.f, 0.f, 0.f, 0.f);

	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_K) == GLFW_PRESS) {
		rb->is_static = true;
		owner->is_physic_active = false;
		owner->transform.setLocalPosition(owner->transform.getLocalPosition() + glm::vec3(0.f, -50.f * deltaTime, 0.f));
	}
	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_I) == GLFW_PRESS) {
		rb->is_static = true;
		owner->is_physic_active = false;
		owner->transform.setLocalPosition(owner->transform.getLocalPosition() + glm::vec3(0.f, 50.f * deltaTime, 0.f));
	}
	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_L) == GLFW_PRESS) {
		rb->is_static = true;
		owner->is_physic_active = false;
		owner->transform.setLocalPosition(owner->transform.getLocalPosition() + glm::vec3(50.f * deltaTime, 0.f, 0.f));
	}
	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_J) == GLFW_PRESS) {
		rb->is_static = true;
		owner->is_physic_active = false;
		owner->transform.setLocalPosition(owner->transform.getLocalPosition() + glm::vec3(-50.f * deltaTime, 0.f, 0.f));
	}

	rb->is_static = false;
	owner->is_physic_active = true;

	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		//std::cout << "Gracz probuje skoczyc" << std::endl;

		if ((rb->groundUnderneath || rb->scaleUnderneath) && canJump) {

			rb->overrideVelocityY = true;
			if (isGravityFlipped) rb->velocityY = -jumpForce;
			else rb->velocityY = jumpForce;

			isJumping = true;
			canJump = false;
		}
	}

	if (virusType != "none") {
		HandleVirus(deltaTime);
	}

	float checkDelay = 0.5f;
	if (gasCheckTimer > checkDelay) {
		if (CheckIfInGas()) {
			gasTimer += checkDelay;

			if (gasTimer > 2.f) {
				Die(false);
			}
		}
		else {
			gasTimer = 0.f;
		}
		gasCheckTimer = 0.f;
	}
	else {
		gasCheckTimer += deltaTime;
	}
}


void PlayerController::onEnd()
{
}

void PlayerController::Die(bool freeze, bool electrified)
{
	if (isDead) return;
	
	timerIndicator->setActive(false);

	if (electrified) {
		rb->is_static = true;
	}
	else if (freeze) {
		Rigidbody* rb = owner->getComponent<Rigidbody>();
		owner->deleteComponent(rb);
	}

	std::shared_ptr<Tag> tag = TagLayerManager::Instance().getTag("Box");
	owner->setTag(tag);
	isDead = true;

	owner->is_animating = false;

	owner->addComponent(std::make_unique<GroundObject>());

	// spawn new player
	Node* playerSpawner = owner->scene_graph->root->getChildByTag("PlayerSpawner");
	playerSpawner->getComponent<PlayerSpawner>()->spawnPlayer();
}

void PlayerController::HandleVirus(float deltaTime)
{
	if (isGravityFlipped) {
		timerIndicator->transform.setLocalPosition(glm::vec3(0.f, -1.f, 0.f));
	}
	else {
		timerIndicator->transform.setLocalPosition(glm::vec3(0.f, 1.f, 0.f));
	}
	
	float smoothing = 10.f; // im wi�ksza, tym szybsze przej�cie

	glm::vec3 currentScale = timerIndicator->transform.getLocalScale();
	glm::vec3 targetScale = glm::vec3(deathTimer * 2.f, 0.2f, 0.2f);

	glm::vec3 newScale = glm::mix(currentScale, targetScale, deltaTime * smoothing);
	timerIndicator->transform.setLocalScale(newScale);
	
	if (abs(rb->velocityX) >= 0.25f || abs(rb->velocityY) >= 1.f) {
		deathTimer = 0.5f;
	}
	else {
		deathTimer -= deltaTime;

		if (deathTimer <= -0.1f) {
			Die(false);
		}
	}
}

bool PlayerController::CheckIfInGas() {
	bool inGas = false;
	
	for (Node* node : owner->scene_graph->root->getAllChildren()) {
		if (!node->in_frustrum) continue;
		//Node* node = box->node;
		if (node->getLayerName() == "Gas") {
			if (ParticleGasNode* gasNode = dynamic_cast<ParticleGasNode*>(node)) {
				glm::vec3 gasPos = gasNode->pos;
				glm::vec3 playerPos = owner->transform.getGlobalPosition();
				isGravityFlipped ? playerPos.y -= 0.5f : playerPos.y += 0.5f;

				float distance = glm::distance(playerPos, gasPos);

				if (distance < 0.8f) {
					inGas = true;
					break;
				}
			}
		}
	}

	return inGas;
}
