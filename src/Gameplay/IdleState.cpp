#include "IdleState.h"
#include "PlayerAnimationController.h"
#include "PlayerController.h"
#include "RunState.h"
#include "JumpState.h"
#include "InAirState.h"
#include "TurnState.h"
#include "../System/Rigidbody.h"
#include "../Basic/Animator.h"

void IdleState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    owner->animator->playAnimation(animation->idle, true);
}

void IdleState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* rb = owner->getComponent<Rigidbody>();
    auto* player = owner->getComponent<PlayerController>();

    if (player->isJumping) {
        animation->changeState(new JumpState());
    }
    else if (animation->facingRight && animation->deltaX < -0.02f) {
        animation->facingRight = false;
        animation->changeState(new TurnState());
    }
    else if (!animation->facingRight && animation->deltaX > 0.02f) {
        animation->facingRight = true;
        animation->changeState(new TurnState());
    }
    else if (abs(animation->deltaX) > 0.02f && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new RunState());
    }
    else if (abs(animation->deltaY) > 0.02f && !(rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new InAirState());
    }
}

void IdleState::exit(Node* owner) {}
