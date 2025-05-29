#include "FallState.h"
#include "PlayerAnimationController.h"
#include "LandState.h"
#include "../../System/Rigidbody.h"
#include "../../Basic/Animator.h"

void FallState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* player = owner->getComponent<PlayerController>();

    player->isJumping = true;

	animation->fall->speed = 600.f;
    owner->animator->blendAnimation(animation->fall, 200.f, false, false);
}

void FallState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* rb = owner->getComponent<Rigidbody>();

    if (rb->groundUnderneath || rb->scaleUnderneath) {
        animation->changeState(new LandState());
    }
}

void FallState::exit(Node* owner) {}
