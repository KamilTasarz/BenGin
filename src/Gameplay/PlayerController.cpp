#include "PlayerController.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "../System/Tag.h"
#include "PlayerSpawner.h"
#include "GroundObject.h"
#include "../Basic/Animator.h"
#include "../System/PhysicsSystem.h"
#include "../Component/BoundingBox.h"
#include "Animation/PlayerAnimationController.h"
#include "GameMath.h"

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
	deathTimer = 0.5f;
 	rb = owner->getComponent<Rigidbody>();
	rb->lockPositionZ = true;
	rb->isPlayer = true;
	//rb->smoothingFactor = 10.f;
	timerIndicator = owner->getChildById(0);
	scale_factor = owner->transform.getLocalScale().x;
	emitter = dynamic_cast<InstanceManager*>(owner->scene_graph->root->getChildByTag("Emitter"));
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

	// DebugMode
	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS && !debugTogglePressed) {
		debugMode = !debugMode;
		debugTogglePressed = true;

		rb->is_static = debugMode;
		owner->setPhysic(!debugMode);
	}

	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_GRAVE_ACCENT) == GLFW_RELEASE) {
		debugTogglePressed = false;
	}

	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_9) == GLFW_PRESS) {
		ServiceLocator::getAudioEngine()->PlaySounds();
	}

	if (debugMode) {
		glm::vec3 move(0.0f);

		if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_W) == GLFW_PRESS)
			move.y += 1.0f;
		if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_S) == GLFW_PRESS)
			move.y -= 1.0f;
		if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_D) == GLFW_PRESS)
			move.x += 1.0f;
		if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_A) == GLFW_PRESS)
			move.x -= 1.0f;

		owner->transform.setLocalPosition(owner->transform.getLocalPosition() + move * 50.f * deltaTime);

		return;
	}

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

	//owner->is_animating = false;
	Animation* death;
	if (GameMath::RandomInt(0, 1) == 0) death = owner->getComponent<PlayerAnimationController>()->deathLeft;
	else death = owner->getComponent<PlayerAnimationController>()->deathRight;
	death->speed = 500.f;
	owner->animator->playAnimation(death, false);

	//owner->addComponent(std::make_unique<GroundObject>());

	// spawn new player
	Node* playerSpawner = owner->scene_graph->root->getChildByTag("PlayerSpawner");
	playerSpawner->getComponent<PlayerSpawner>()->spawnPlayer();
}

void PlayerController::HandleVirus(float deltaTime)
{
	/*if (isGravityFlipped) {
		timerIndicator->transform.setLocalPosition(glm::vec3(0.f, 0.f, -1.f));
	}
	else {
		timerIndicator->transform.setLocalPosition(glm::vec3(0.f, 0.f, 1.f));
	}*/
	
	float smoothing = 10.f;

	glm::vec3 currentScale = timerIndicator->transform.getLocalScale();
	glm::vec3 targetScale = glm::vec3(0.2f, deathTimer * 4.f, 0.2f);

	glm::vec3 newScale = glm::mix(currentScale, targetScale, deltaTime * smoothing);
	timerIndicator->transform.setLocalScale(newScale);
	
	std::cout << "Gracz biegnie: " << isRunning << ", gracz skacze: " << isJumping << std::endl;

	if (/*abs(rb->velocityX) >= 0.25f || abs(rb->velocityY) >= 2.f*/ isRunning || !rb->groundUnderneath) {
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
	
	float minDist = 1000.f, dist = 0.f;

	int index = emitter->tail - 1, iterator = 0;
	glm::vec3 playerPos = owner->transform.getGlobalPosition();
	isGravityFlipped ? playerPos.y -= 0.5f : playerPos.y += 0.5f;

	for (int i = 0; i < emitter->size; i++) {
		dist = glm::distance(glm::vec2(emitter->particles[index].position), glm::vec2(playerPos));
		if (dist < 0.8f) {
			
			inGas = true;
			break;
			
		}
		index--;
		if (index < 0) {
			index += emitter->max_size;
		}
		if (dist < minDist) {
			minDist = dist;
			iterator = 0;
		}

		if (iterator > 30) {
			break;
		}
		else {
			iterator++;
		}
	}

	return inGas;
}
