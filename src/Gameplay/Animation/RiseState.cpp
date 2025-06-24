#include "RiseState.h"
#include "PlayerAnimationController.h"
#include "FallState.h"
#include "../../System/Rigidbody.h"
#include "../../Basic/Animator.h"

void RiseState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    animation->rise->speed = 500.f;
    owner->animator->blendAnimation(animation->rise, 50.f, true, false);
}

void RiseState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
	auto* rb = owner->getComponent<Rigidbody>();

    if (abs(animation->deltaY) < (4.f * deltaTime) && !rb->overrideVelocityY) {
        animation->changeState(new FallState());
    }
}

void RiseState::exit(Node* owner) {}

std::string RiseState::getName() const {
    return "RiseState";
}