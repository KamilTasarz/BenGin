#include "AnimationRewindable.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "Animation/PlayerAnimationController.h"
#include "../Basic/Animator.h"

#include "Animation/IdleState.h"

REGISTER_SCRIPT(AnimationRewindable);

void AnimationRewindable::onUpdate(float deltaTime)
{
	if (!owner) return;

	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_R) == GLFW_PRESS) {
		isRewinding = true;
	}
	else isRewinding = false;

	if (isRewinding) {
		for (int i = 0; i < rewindSpeed; ++i) {
			if (!playerHistory.empty()) {
				AnimationSnapshot snap = playerHistory.back();
				playerHistory.pop_back();

				animationController->isTurning = snap.isTurning;
				animationController->targetRotation = snap.targetRotation;
				animationController->gravityFlipped = snap.gravityFlipped;
				animationController->facingRight = snap.facingRight;
				animationController->getOwner()->animator->current_animation = snap.currentAnimation;
			}
		}
	}
	else {
		AnimationSnapshot snap;

		snap.isTurning = animationController->isTurning;
		snap.targetRotation = animationController->targetRotation;
		snap.gravityFlipped = animationController->gravityFlipped;
		snap.facingRight = animationController->facingRight;
		snap.currentAnimation = animationController->getOwner()->animator->current_animation;

		playerHistory.push_back(snap);
	}

	/*if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_R) == GLFW_RELEASE) {
		animationController->changeState(new IdleState());
	}*/
}
