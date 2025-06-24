#include "RunState.h"
#include "PlayerAnimationController.h"
#include "../PlayerController.h"
#include "JumpState.h"
#include "IdleState.h"
#include "FallState.h"
#include "PushState.h"
#include "../../System/Rigidbody.h"
#include "../../Basic/Animator.h"
#include "../GameManager.h"
#include "../Particles.h"

void RunState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* player = owner->getComponent<PlayerController>();
    animation->run->speed = player->virusType == "black" ? 700.f : 1000.f;
    owner->animator->blendAnimation(animation->run, 50.f, true, true);

    auto* audio = ServiceLocator::getAudioEngine();
    sfxId = audio->PlayMusic(audio->running, GameManager::instance().sfxVolume * 75.f);

	owner->getChildByNamePart("run_particles")->getComponent<Particles>()->emit = true;
}

void RunState::update(Node* owner, float deltaTime) {
    auto* animation = owner->getComponent<PlayerAnimationController>();
    auto* rb = owner->getComponent<Rigidbody>();
    auto* player = owner->getComponent<PlayerController>();

    if (player->isJumping) {
        animation->changeState(new JumpState());
    }
    else if (rb->isPushing && player->isRunning) {
        animation->changeState(new PushState());
    }
    else if (/*abs(animation->deltaX) < (4.f * deltaTime) && abs(rb->velocityDeltaX) < 0.2f*/ !player->isRunning && (rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new IdleState());
    }
    else if (/*abs(rb->velocityY) > 0.2f*/ abs(animation->deltaY) > 0.f && !(rb->groundUnderneath || rb->scaleUnderneath)) {
        animation->changeState(new FallState());
    }
}

void RunState::exit(Node* owner) {
    auto* audio = ServiceLocator::getAudioEngine();
    audio->stopSound(sfxId);
    sfxId = -1;

    owner->getChildByNamePart("run_particles")->getComponent<Particles>()->emit = false;
}

std::string RunState::getName() const {
    return "RunState";
}