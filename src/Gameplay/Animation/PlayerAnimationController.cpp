#include "PlayerAnimationController.h"
#include "../../Basic/Node.h"
#include "../../Basic/Animator.h"
#include "../../Basic/Model.h"
#include "../RegisterScript.h"
#include "IPlayerAnimState.h"

#include "FallState.h"
#include "JumpState.h"
#include "RiseState.h"
#include "LandState.h"
#include "IdleState.h"
#include "RunState.h"
#include "DeathState.h"
#include "../GameManager.h"

REGISTER_SCRIPT(PlayerAnimationController);

void PlayerAnimationController::onAttach(Node* owner)
{
	this->owner = owner;
	//animator = owner->animator;
}

void PlayerAnimationController::onDetach()
{
	this->owner = nullptr;
	//animator = nullptr;
}

void PlayerAnimationController::onStart()
{
	rb = owner->getComponent<Rigidbody>();
	player = owner->getComponent<PlayerController>();
	previousPosition = owner->transform.getLocalPosition();
	facingRight = true;
	allFinished = false;

	if (!owner->is_animating) {
		owner->is_animating = true;
	}
	
	idle = owner->pModel->getAnimationByName("Idle");
	turn = owner->pModel->getAnimationByName("Turn");
	run = owner->pModel->getAnimationByName("Run");
	sleep = owner->pModel->getAnimationByName("Sleep");
	jump = owner->pModel->getAnimationByName("JumpStart");
	//inAir = owner->pModel->getAnimationByName("JumpInAir");
	rise = owner->pModel->getAnimationByName("JumpUp");
	fall = owner->pModel->getAnimationByName("JumpFall");
	land = owner->pModel->getAnimationByName("JumpEnd");
	push = owner->pModel->getAnimationByName("PushFinal");
	deathLeft = owner->pModel->getAnimationByName("DeadBackTailLeft");
	deathRight = owner->pModel->getAnimationByName("DeadBackTailRight");

	changeState(new IdleState());

	owner->animator->blendAnimation(sleep, 50.f, false, false);

	owner->animator->current_animation;

	rewindable = owner->getComponent<AnimationRewindable>();
	if (rewindable == nullptr) {
		owner->addComponent(std::make_unique<AnimationRewindable>());
		rewindable = owner->getComponent<AnimationRewindable>();
	}
}

void PlayerAnimationController::onUpdate(float deltaTime)
{
	bool isDead = owner->getComponent<PlayerController>()->isDead;
	if (isDead && allFinished) {
		
		/*if (currentState && GameManager::instance->currentPlayer == owner)
			changeState(new DeathState());*/
		
		//if (currentState)
		//	currentState->update(owner, deltaTime);

		/*if (currentState) {
			currentState->exit(owner);
			delete currentState;
			currentState = nullptr;
		}*/
		
		return;
	}

	if (isDead && !allFinished && !dynamic_cast<DeathState*>(currentState)) {
		changeState(new DeathState());
	}
	
	deltaX = owner->transform.getLocalPosition().x - previousPosition.x;
	deltaY = owner->transform.getLocalPosition().y - previousPosition.y;
	
	if (currentState)
		currentState->update(owner, deltaTime);

	if (player->pressedLeft && facingRight) {
		StartRotation(facingRight, false, 180.f, glm::vec3(0.f, 1.f, 0.f));
	}
	else if (player->pressedRight && !facingRight) {
		StartRotation(facingRight, true, -180.f, glm::vec3(0.f, 1.f, 0.f));
	}

	bool desiredGravityState = owner->getComponent<PlayerController>()->isGravityFlipped;
	if (gravityFlipped != desiredGravityState) {
		StartRotation(gravityFlipped, desiredGravityState, 180.f, glm::vec3(1.f, 0.f, 0.f));
	}

	UpdateRotation(deltaTime);

	if ((glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_Z) == GLFW_PRESS)) {
		owner->animator->blendAnimation(idle, 100.f, true, true);
	}
	if ((glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_X) == GLFW_PRESS)) {
		owner->animator->blendAnimation(turn, 100.f, true, true);
	}
	if ((glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_C) == GLFW_PRESS)) {
		owner->animator->blendAnimation(run, 100.f, true, true);
	}
	if ((glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_V) == GLFW_PRESS)) {
		owner->animator->blendAnimation(sleep, 100.f, true, true);
	}
	if ((glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_B) == GLFW_PRESS)) {
		owner->animator->blendAnimation(jump, 100.f, true, true);
	}
	if ((glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_N) == GLFW_PRESS)) {
		owner->animator->blendAnimation(inAir, 100.f, true, true);
	}
	if ((glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_M) == GLFW_PRESS)) {
		owner->animator->blendAnimation(fall, 100.f, true, true);
	}
	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_H) == GLFW_PRESS) {
		owner->animator->blendAnimation(push, 100.f, true, true);
	}

	previousPosition = owner->transform.getLocalPosition();
}

void PlayerAnimationController::onEnd()
{
	delete currentState;
}

void PlayerAnimationController::changeState(IPlayerAnimState* newState) {
	if (currentState)
		currentState->exit(owner);

	delete currentState;
	currentState = newState;

	if (currentState)
		currentState->enter(owner);
}

void PlayerAnimationController::StartRotation(bool& conditionFlag, bool desiredState, float angleDegrees, const glm::vec3& axis)
{
	if (!isTurning && conditionFlag != desiredState) {
		conditionFlag = desiredState;
		isTurning = true;
		targetRotation = glm::angleAxis(glm::radians(angleDegrees), axis) * owner->transform.getLocalRotation();
	}
}

void PlayerAnimationController::UpdateRotation(float deltaTime)
{
	if (!isTurning)
		return;

	glm::quat currentRotation = owner->transform.getLocalRotation();
	glm::quat newRotation = glm::slerp(currentRotation, targetRotation, turnSpeed * deltaTime);
	owner->transform.setLocalRotation(newRotation);

	float dot = glm::dot(glm::normalize(newRotation), glm::normalize(targetRotation));
	if (abs(dot) > 0.999f) {
		owner->transform.setLocalRotation(targetRotation);
		isTurning = false;
	}
}

