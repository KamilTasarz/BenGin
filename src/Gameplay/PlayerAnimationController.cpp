#include "PlayerAnimationController.h"
#include "../Basic/Node.h"
#include "../Basic/Animator.h"
#include "../Basic/Model.h"      // Potrzebne dla owner->pModel
#include "RegisterScript.h"

// Spróbuj uporz¹dkowaæ tak, aby najpierw by³y te, które s¹ "najmniej zale¿ne"
// lub te, które s¹ potrzebne jako pierwsze. To jednak nie gwarantuje sukcesu.
#include "IPlayerAnimState.h" // Zawsze jako pierwszy z interfejsów/stanów

// Stany, które nie tworz¹ tak wielu innych stanów lub s¹ koñcowe/pocz¹tkowe
#include "FallState.h"   // Tworzy LandState
#include "JumpState.h"    // Tworzy InAirState
#include "RiseState.h"    // Tworzy InAirState

// Stany bardziej "centralne" lub z wiêksz¹ liczb¹ przejœæ
#include "LandState.h"    // Tworzy Idle, Run, Jump
#include "IdleState.h"    // Tworzy Run, Jump, InAir
#include "RunState.h"     // Tworzy Jump, Idle


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
	//fall = owner->pModel->getAnimationByName("JumpEnd");

	changeState(new IdleState());
}

void PlayerAnimationController::onUpdate(float deltaTime)
{
	if (owner->getComponent<Rigidbody>() == nullptr) {
		return;
	}
	
	deltaX = owner->transform.getLocalPosition().x - previousPosition.x;
	deltaY = owner->transform.getLocalPosition().y - previousPosition.y;
	
	if (abs(deltaX) < (4.f * deltaTime) && abs(rb->velocityDeltaX) < 0.2f && (rb->groundUnderneath || rb->scaleUnderneath)) {
		isStanding = true;
	}
	else if (abs(rb->velocityX) > 0.2f && abs(rb->velocityDeltaX) >= 0.2f && (rb->groundUnderneath || rb->scaleUnderneath)) {
		isRunning = true;
	}

	if (player->isJumping) {
		hasJumped = true;
	}
	else if (rb->groundUnderneath || rb->scaleUnderneath) {
		hasLanded = true;
	}

	if ((rb->velocityDeltaY < 0.f && !gravityFlipped) || (rb->velocityDeltaY > 0.f && gravityFlipped)) {
		isFalling = true;
	}
	else {
		isFalling = false;
	}

	if (currentState)
		currentState->update(owner, deltaTime);

	glm::vec3 rotationAxis(0.f, 1.f, 0.f);

	if (!isTurning) {
		if (facingRight && deltaX < -(4.f * deltaTime)) {
			facingRight = false;
			isTurning = true;
			targetRotation = glm::angleAxis(glm::radians(180.f), rotationAxis) * owner->transform.getLocalRotation();
		}
		else if (!facingRight && deltaX > (4.f * deltaTime)) {
			facingRight = true;
			isTurning = true;
			targetRotation = glm::angleAxis(glm::radians(-180.f), rotationAxis) * owner->transform.getLocalRotation();
		}
	}

	if (isTurning) {
		glm::quat currentRotation = owner->transform.getLocalRotation();
		glm::quat newRotation = glm::slerp(currentRotation, targetRotation, turnSpeed * deltaTime);

		owner->transform.setLocalRotation(newRotation);

		float dot = glm::dot(glm::normalize(newRotation), glm::normalize(targetRotation));
		if (abs(dot) > 0.999f) {
			owner->transform.setLocalRotation(targetRotation);
			isTurning = false;
		}
	}

	if (owner->getComponent<PlayerController>()->isGravityFlipped && !gravityFlipped) {
		glm::vec3 newScale = owner->transform.getLocalScale() * glm::vec3(1.f, -1.f, 1.f);
		owner->transform.setLocalScale(newScale);
		gravityFlipped = true;
	}
	else if (!owner->getComponent<PlayerController>()->isGravityFlipped && gravityFlipped) {
		glm::vec3 newScale = owner->transform.getLocalScale() * glm::vec3(1.f, -1.f, 1.f);
		owner->transform.setLocalScale(newScale);
		gravityFlipped = false;
	}

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
		owner->animator->blendAnimation(run, 100.f, true, true);
	}

	if (rb->targetVelocityX > 0.f) {
		std::cout << "gracz siê porusza" << std::endl;
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

