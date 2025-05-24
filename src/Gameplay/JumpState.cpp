#include "JumpState.h"
#include "PlayerAnimationController.h"
#include "InAirState.h"
#include "../System/Rigidbody.h"
#include "../Basic/Animator.h"

void JumpState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    animation->jump->speed = 1500.f;
    owner->animator->blendAnimation(animation->jump, 10.f, true, false);
}

void JumpState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();

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

    if (owner->animator->isPlayingNonLooping()) {
        return;
    }

    if (abs(animation->deltaY) < (4.f * deltaTime)) {
        animation->changeState(new InAirState());
    }
}

void JumpState::exit(Node* owner) {}
