#include "RunState.h"
#include "PlayerAnimationController.h"
#include "PlayerController.h"
#include "JumpState.h"
#include "IdleState.h"
#include "FallState.h"
#include "TurnState.h"
#include "../System/Rigidbody.h"
#include "../Basic/Animator.h"

void RunState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    animation->run->speed = 1000.f;
    owner->animator->blendAnimation(animation->run, 50.f, true, true);
}

void RunState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* rb = owner->getComponent<Rigidbody>();
    auto* player = owner->getComponent<PlayerController>();

    if (player->isJumping) {
        animation->changeState(new JumpState());
    }
    /*else if (animation->facingRight && animation->deltaX < -(4.f * deltaTime)) {
        animation->facingRight = false;
        animation->changeState(new TurnState());
    }
    else if (!animation->facingRight && animation->deltaX > (4.f * deltaTime)) {
        animation->facingRight = true;
        animation->changeState(new TurnState());
    }*/
    else if (abs(animation->deltaX) < (4.f * deltaTime) && abs(rb->velocityDeltaX) < 0.2f && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new IdleState());
    }
    else if (/*abs(rb->velocityY) > 0.2f*/ abs(rb->velocityDeltaY) > 0.f && !(rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new FallState());
    }
}

void RunState::exit(Node* owner) {}
