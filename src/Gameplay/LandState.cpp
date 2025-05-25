#include "LandState.h"
#include "PlayerAnimationController.h"
#include "PlayerController.h"
#include "IdleState.h"
#include "RunState.h"
#include "JumpState.h"
#include "../System/Rigidbody.h"
#include "../Basic/Animator.h"

void LandState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    animation->land->speed = 1500.f;
    owner->animator->playAnimation(animation->land, false);
    //owner->animator->blendAnimation(animation->fall, 50.f, true, false);
}

void LandState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* rb = owner->getComponent<Rigidbody>();
    auto* player = owner->getComponent<PlayerController>();

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

	if (owner->animator->isPlayingNonLooping()) {
		return;
	}

    if (player->isJumping) {
        animation->changeState(new JumpState());
    }
    else if (abs(animation->deltaX) < (4.f * deltaTime) && abs(rb->velocityDeltaX) < 0.2f && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new IdleState());
    }
    else if (abs(rb->velocityX) > 0.2f && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new RunState());
    }
}

void LandState::exit(Node* owner) {
    PlayerController* p = owner->getComponent<PlayerController>();
    p->canJump = true;
}
