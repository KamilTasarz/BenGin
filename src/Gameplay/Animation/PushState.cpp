#include "PushState.h"
#include "PlayerAnimationController.h"
#include "../PlayerController.h"
#include "RunState.h"
#include "JumpState.h"
#include "FallState.h"
#include "IdleState.h"
#include "../../System/Rigidbody.h"
#include "../../Basic/Animator.h"

//PlayerAnimationController* animation;

void PushState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    //animation->idle->speed = 500.f;
    owner->animator->blendAnimation(animation->push, 100.f, false, true);
}

void PushState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* rb = owner->getComponent<Rigidbody>();
    auto* player = owner->getComponent<PlayerController>();

    if (player->isJumping) {
        animation->changeState(new JumpState());
        return;
    }
    else if (/*abs(animation->deltaX) < (4.f * deltaTime) && abs(rb->velocityDeltaX) < 0.2f*/ !player->isRunning && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new IdleState());
    }
   
	if (rb->isPushing) {
		return;
	}
    
    if (abs(rb->velocityX) > 0.2f && abs(rb->velocityDeltaX) >= 0.2f && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new RunState());
    }
    else if (abs(rb->velocityDeltaY) > 0.f && !(rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new FallState());
    }
}

void PushState::exit(Node* owner) {}
