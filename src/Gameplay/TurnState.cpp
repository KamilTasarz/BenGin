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
    animation->turn->speed = 8000.f;
    owner->animator->blendAnimation(animation->turn, 500.f, true, false);
}

void TurnState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* rb = owner->getComponent<Rigidbody>();
    auto* player = owner->getComponent<PlayerController>();

    if (owner->animator->isPlayingNonLooping()) {
        return;
    }

    if (!animation->facingRight) {
        glm::vec3 newScale = owner->transform.getLocalScale() * glm::vec3(1.f, -1.f, 1.f);
        owner->transform.setLocalScale(newScale);

		glm::vec3 position = owner->transform.getLocalPosition();
		position.x -= 0.78f;
		owner->transform.setLocalPosition(position);
    }
    else if (animation->facingRight) {
        glm::vec3 newScale = owner->transform.getLocalScale() * glm::vec3(1.f, -1.f, 1.f);
        owner->transform.setLocalScale(newScale);

        glm::vec3 position = owner->transform.getLocalPosition();
        position.x += 0.78f;
        owner->transform.setLocalPosition(position);
    }

    if (player->isJumping) {
        animation->changeState(new JumpState());
    }
    else if (abs(rb->velocityX) < 0.2f && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new IdleState());
    }
    else if (abs(rb->velocityX) > 0.2f && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new RunState());
    }
}

void TurnState::exit(Node* owner) {}
