#include "FallState.h"
#include "PlayerAnimationController.h"
#include "LandState.h"
#include "../../System/Rigidbody.h"
#include "../../Basic/Animator.h"

void FallState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
	animation->fall->speed = 600.f;
    //owner->animator->playAnimation(animation->inAir, false);
    owner->animator->blendAnimation(animation->fall, 200.f, false, false);
}

void FallState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* rb = owner->getComponent<Rigidbody>();

   /* if (animation->facingRight && animation->deltaX < -(4.f * deltaTime)) {
        animation->facingRight = false;
        glm::vec3 newScale = owner->transform.getLocalScale() * glm::vec3(1.f, -1.f, 1.f);
        owner->transform.setLocalScale(newScale);
    }
    else if (!animation->facingRight && animation->deltaX > (4.f * deltaTime)) {
        animation->facingRight = true;
        glm::vec3 newScale = owner->transform.getLocalScale() * glm::vec3(1.f, -1.f, 1.f);
        owner->transform.setLocalScale(newScale);
    }*/

    if (rb->groundUnderneath || rb->scaleUnderneath) {
        animation->changeState(new LandState());
    }
}

void FallState::exit(Node* owner) {}
