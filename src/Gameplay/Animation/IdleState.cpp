#include "IdleState.h"
#include "PlayerAnimationController.h"
#include "../PlayerController.h"
#include "RunState.h"
#include "JumpState.h"
#include "FallState.h"
#include "PushState.h"
#include "../../System/Rigidbody.h"
#include "../../Basic/Animator.h"

void IdleState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    animation->idle->speed = 500.f;
    owner->animator->blendAnimation(animation->idle, 100.f, false, true);
}

void IdleState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* rb = owner->getComponent<Rigidbody>();
    auto* player = owner->getComponent<PlayerController>();

    if (player->isJumping) {
        animation->changeState(new JumpState());
    }
    else if (rb->isPushing && player->isRunning) {
		animation->changeState(new PushState());
	}
    else if (/*abs(rb->velocityX) > 0.2f && abs(rb->velocityDeltaX) >= 0.2f*/ player->isRunning && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new RunState());
    }
    else if (/*abs(rb->velocityY) > 0.02f*/ abs(animation->deltaY) < (4.f * deltaTime) && !(rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new FallState());
    }
}

void IdleState::exit(Node* owner) {}

std::string IdleState::getName() const {
    return "IdleState";
}