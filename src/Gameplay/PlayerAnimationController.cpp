#include "PlayerAnimationController.h"
#include "PlayerController.h"
#include "../Basic/Node.h"
#include "../Basic/Animator.h"
#include "../Basic/Model.h"      // Potrzebne dla owner->pModel
#include "../System/Rigidbody.h" // Potrzebne dla owner->getComponent<Rigidbody>()
#include "RegisterScript.h"

// Spróbuj uporz¹dkowaæ tak, aby najpierw by³y te, które s¹ "najmniej zale¿ne"
// lub te, które s¹ potrzebne jako pierwsze. To jednak nie gwarantuje sukcesu.
#include "IPlayerAnimState.h" // Zawsze jako pierwszy z interfejsów/stanów

// Stany, które nie tworz¹ tak wielu innych stanów lub s¹ koñcowe/pocz¹tkowe
#include "InAirState.h"   // Tworzy LandState
#include "JumpState.h"    // Tworzy InAirState

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
	inAir = owner->pModel->getAnimationByName("JumpInAir");
	fall = owner->pModel->getAnimationByName("JumpEnd");

	changeState(new IdleState());
}

void PlayerAnimationController::onUpdate(float deltaTime)
{
	deltaX = owner->transform.getLocalPosition().x - previousPosition.x;
	deltaY = owner->transform.getLocalPosition().y - previousPosition.y;
	
	if (currentState)
		currentState->update(owner, deltaTime);

	if (facingRight && deltaX < -0.02f) {
		facingRight = false;
		glm::vec3 newScale = owner->transform.getLocalScale() * glm::vec3(1.f,-1.f, 1.f);
		owner->transform.setLocalScale(newScale);
	}
	else if (!facingRight && deltaX > 0.02f) {
		facingRight = true;
		glm::vec3 newScale = owner->transform.getLocalScale() * glm::vec3(1.f, -1.f, 1.f);
		owner->transform.setLocalScale(newScale);
	}

	if (owner->getComponent<PlayerController>()->isGravityFlipped && !gravityFlipped) {
		glm::vec3 newScale = owner->transform.getLocalScale() * glm::vec3(1.f, 1.f, -1.f);
		owner->transform.setLocalScale(newScale);
		gravityFlipped = true;
	}
	else if (!owner->getComponent<PlayerController>()->isGravityFlipped && gravityFlipped) {
		glm::vec3 newScale = owner->transform.getLocalScale() * glm::vec3(1.f, 1.f, -1.f);
		owner->transform.setLocalScale(newScale);
		gravityFlipped = false;
	}

	if ((glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_Z) == GLFW_PRESS)) {
		owner->animator->playAnimation(idle, false);
	}
	if ((glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_X) == GLFW_PRESS)) {
		owner->animator->playAnimation(turn, false);
	}
	if ((glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_C) == GLFW_PRESS)) {
		owner->animator->playAnimation(run, false);
	}
	if ((glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_V) == GLFW_PRESS)) {
		owner->animator->playAnimation(sleep, false);
	}
	if ((glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_B) == GLFW_PRESS)) {
		owner->animator->playAnimation(jump, false);
	}
	if ((glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_N) == GLFW_PRESS)) {
		owner->animator->playAnimation(inAir, false);
	}
	if ((glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_M) == GLFW_PRESS)) {
		owner->animator->playAnimation(fall, false);
	}
	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_H) == GLFW_PRESS) {
		owner->animator->blendAnimation(run, 0.5f, true);
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

