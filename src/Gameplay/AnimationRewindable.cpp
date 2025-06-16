#include "AnimationRewindable.h"
#include "../Basic/Node.h"
#include "Animation/PlayerAnimationController.h"
#include "../Basic/Animator.h"
#include "RegisterScript.h"
#include "RewindManager.h"
#include "GameManager.h"
#include "GameManagerRewindable.h"
#include "Animation/IdleState.h"

REGISTER_SCRIPT(AnimationRewindable);

void AnimationRewindable::onAttach(Node* owner) {
	this->owner = owner;
    RewindManager::Instance().registerRewindable(this);
}

void AnimationRewindable::onDetach() {
	animationController = nullptr;
	owner = nullptr;
	RewindManager::Instance().unregisterRewindable(this);
}

void AnimationRewindable::onStart() {
    animationController = owner->getComponent<PlayerAnimationController>();
}

std::shared_ptr<ITimeSnapshot> AnimationRewindable::createSnapshot() {
    auto snap = std::make_shared<AnimationSnapshot>();
    snap->isTurning = animationController->isTurning;
    snap->targetRotation = animationController->targetRotation;
    snap->gravityFlipped = animationController->gravityFlipped;
    snap->facingRight = animationController->facingRight;
    snap->currentAnimation = animationController->getOwner()->animator->current_animation;
    return snap;
}

void AnimationRewindable::applySnapshot(const std::shared_ptr<ITimeSnapshot>& baseSnap) {
    auto snap = std::dynamic_pointer_cast<AnimationSnapshot>(baseSnap);
    if (!snap) return;

    animationController->isTurning = snap->isTurning;
    animationController->targetRotation = snap->targetRotation;
    animationController->gravityFlipped = snap->gravityFlipped;
    animationController->facingRight = snap->facingRight;
    animationController->getOwner()->animator->current_animation = snap->currentAnimation;

    /*if (history.empty() && GameManager::instance->rewindable->history.size() > 1) {
        animationController->getOwner()->animator->current_animation = animationController->idle;
        animationController->changeState(new IdleState());
    }*/
}
