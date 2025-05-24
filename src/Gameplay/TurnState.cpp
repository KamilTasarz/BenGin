#include "TurnState.h"
#include "PlayerAnimationController.h"
#include "PlayerController.h"
#include "IdleState.h"
#include "RunState.h"
#include "JumpState.h"
#include "../System/Rigidbody.h"
#include "../Basic/Animator.h"

void TurnState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    animation->fall->speed = 4000.f;
    owner->animator->playAnimation(animation->turn, false);
}

void TurnState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* rb = owner->getComponent<Rigidbody>();
    auto* player = owner->getComponent<PlayerController>();

    if (owner->animator->isPlayingNonLooping()) {
        return;
    }

    if (player->isJumping) {
        animation->changeState(new JumpState());
    }
    else if (abs(animation->deltaX) < 0.02f && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new IdleState());
    }
    else if (abs(animation->deltaX) > 0.02f && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new RunState());
    }
}

void TurnState::exit(Node* owner) {}
