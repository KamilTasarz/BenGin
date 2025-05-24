#include "RunState.h"
#include "PlayerAnimationController.h"
#include "PlayerController.h"
#include "JumpState.h"
#include "IdleState.h"
#include "InAirState.h"
#include "../System/Rigidbody.h"
#include "../Basic/Animator.h"

void RunState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    animation->run->speed = 2000.f;
    owner->animator->playAnimation(animation->run, true);
}

void RunState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* rb = owner->getComponent<Rigidbody>();
    auto* player = owner->getComponent<PlayerController>();

    if (player->isJumping) {
        animation->changeState(new JumpState());
    }
    else if (abs(animation->deltaX) < 0.02f && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new IdleState());
    }
    else if (abs(animation->deltaY) > 0.02f && !(rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new InAirState());
    }
}

void RunState::exit(Node* owner) {}
