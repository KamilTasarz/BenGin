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
    animation->push->speed = 1500.f;
    owner->animator->blendAnimation(animation->push, 200.f, false, true);
}

void PushState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* rb = owner->getComponent<Rigidbody>();
    auto* player = owner->getComponent<PlayerController>();

    /*float speed = abs(rb->velocityX) * 350.f;
    if (speed < 150.f) speed = 0.f;
    animation->push->speed = speed;*/

    if (player->isJumping) {
        animation->changeState(new JumpState());
        return;
    }
    else if (!player->isRunning && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new IdleState());
    }
   
	if (rb->isPushing) {
		return;
	}
    
    if (player->isRunning && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new RunState());
    }
    else if (abs(rb->velocityDeltaY) > 0.f && !(rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new FallState());
    }
}

void PushState::exit(Node* owner) {}

std::string PushState::getName() const {
    return "PushState";
}