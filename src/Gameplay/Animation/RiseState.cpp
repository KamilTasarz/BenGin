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

    /*if (animation->facingRight && animation->deltaX < -(4.f * deltaTime)) {
        animation->facingRight = false;
        glm::vec3 newScale = owner->transform.getLocalScale() * glm::vec3(1.f, -1.f, 1.f);
        owner->transform.setLocalScale(newScale);
    }
    else if (!animation->facingRight && animation->deltaX > (4.f * deltaTime)) {
        animation->facingRight = true;
        glm::vec3 newScale = owner->transform.getLocalScale() * glm::vec3(1.f, -1.f, 1.f);
        owner->transform.setLocalScale(newScale);
    }*/

    if (abs(animation->deltaY) < (4.f * deltaTime)) {
        animation->changeState(new FallState());
    }
}

void RiseState::exit(Node* owner) {}
