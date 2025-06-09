#include "LandState.h"
#include "PlayerAnimationController.h"
#include "../PlayerController.h"
#include "IdleState.h"
#include "RunState.h"
#include "JumpState.h"
#include "../../System/Rigidbody.h"
#include "../../Basic/Animator.h"
#include "../GameManager.h"

void LandState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    animation->land->speed = 1500.f;
    owner->animator->blendAnimation(animation->fall, 50.f, true, false);

    auto* audio = ServiceLocator::getAudioEngine();
    audio->PlaySFX(audio->landing, GameManager::instance->sfxVolume * 90.f);
}

void LandState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* rb = owner->getComponent<Rigidbody>();
    auto* player = owner->getComponent<PlayerController>();

	/*if (owner->animator->isPlayingNonLooping()) {
		return;
	}*/

    if (player->isJumping) {
        animation->changeState(new JumpState());
    }
    else if (/*abs(animation->deltaX) < (4.f * deltaTime) && abs(rb->velocityDeltaX) < 0.2f*/ !player->isRunning && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new IdleState());
    }
    else if (abs(rb->velocityX) > 0.2f /*!player->isRunning*/ && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new RunState());
    }
}

void LandState::exit(Node* owner) {
    PlayerController* p = owner->getComponent<PlayerController>();
    p->canJump = true;
}
