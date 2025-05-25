#include "InAirState.h"
#include "PlayerAnimationController.h"
#include "LandState.h"
#include "../System/Rigidbody.h"
#include "../Basic/Animator.h"

void InAirState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
	animation->inAir->speed = 100.f;
    owner->animator->blendAnimation(animation->inAir, 200.f, false, false);
}

void InAirState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* rb = owner->getComponent<Rigidbody>();

    if (animation->facingRight && animation->deltaX < -(4.f * deltaTime)) {
        animation->facingRight = false;
        glm::vec3 newScale = owner->transform.getLocalScale() * glm::vec3(1.f, -1.f, 1.f);
        owner->transform.setLocalScale(newScale);
    }
    else if (!animation->facingRight && animation->deltaX > (4.f * deltaTime)) {
        animation->facingRight = true;
        glm::vec3 newScale = owner->transform.getLocalScale() * glm::vec3(1.f, -1.f, 1.f);
        owner->transform.setLocalScale(newScale);
    }

    if (rb->groundUnderneath || rb->scaleUnderneath) {
        animation->changeState(new LandState());
    }
}

void InAirState::exit(Node* owner) {}
