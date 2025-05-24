#include "InAirState.h"
#include "PlayerAnimationController.h"
#include "LandState.h"
#include "../System/Rigidbody.h"
#include "../Basic/Animator.h"

void InAirState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
	animation->inAir->speed = 250.f;
    owner->animator->playAnimation(animation->inAir, false);
}

void InAirState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* rb = owner->getComponent<Rigidbody>();

    if (rb->groundUnderneath || rb->scaleUnderneath) {
        animation->changeState(new LandState());
    }
}

void InAirState::exit(Node* owner) {}
