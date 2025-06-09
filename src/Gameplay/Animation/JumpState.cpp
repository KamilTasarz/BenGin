#include "JumpState.h"
#include "PlayerAnimationController.h"
#include "RiseState.h"
#include "../../System/Rigidbody.h"
#include "../../Basic/Animator.h"
#include "../GameManager.h"

void JumpState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    animation->jump->speed = 1500.f;
    owner->animator->blendAnimation(animation->jump, 50.f, true, false);

    auto* audio = ServiceLocator::getAudioEngine();
    audio->PlaySFX(audio->jumping, GameManager::instance->sfxVolume * 95.f);
}

void JumpState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();

    if (owner->animator->isPlayingNonLooping()) {
        return;
    }

    animation->changeState(new RiseState());
}

void JumpState::exit(Node* owner) {}
