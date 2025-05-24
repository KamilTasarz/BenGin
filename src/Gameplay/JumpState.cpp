#include "JumpState.h"
#include "PlayerAnimationController.h"
#include "InAirState.h"
#include "../System/Rigidbody.h"
#include "../Basic/Animator.h"

void JumpState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    animation->jump->speed = 1500.f;
    owner->animator->playAnimation(animation->jump, false);
}

void JumpState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();

    if (owner->animator->isPlayingNonLooping()) {
        return;
    }

    if (abs(animation->deltaY) < 0.02f) {
        animation->changeState(new InAirState());
    }
}

void JumpState::exit(Node* owner) {}
