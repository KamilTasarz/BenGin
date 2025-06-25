#include "DeathState.h"
#include "PlayerAnimationController.h"
#include "../../System/Rigidbody.h"
#include "../../Basic/Animator.h"
#include "../../Basic/Model.h"
#include "../GameManager.h"
#include "../GameMath.h"
#include "../../ResourceManager.h"

void DeathState::enter(Node* owner) {
    auto* animation = owner->getComponent<PlayerAnimationController>();

	Animation* death = nullptr;
    if (GameMath::RandomInt(0, 1) == 0) death = animation->deathLeft;
    else death = animation->deathRight;

    death->speed = 500.f;
    owner->animator->blendAnimation(death, 50.f, true, false);
}

void DeathState::update(Node* owner, float deltaTime) {
    
    if (owner->animator->ended) exit(owner);
    
    if (owner->animator->isPlayingNonLooping()) {
        return;
    }
}

void DeathState::exit(Node* owner) {
    auto newTexture = owner->pModel->meshes[0].textures[0];

    //reset rat texture
    auto ratTexture = ResourceManager::Instance().getTexture(34);
    owner->pModel->meshes[0].textures[0] = (ratTexture);

    auto* animation = owner->getComponent<PlayerAnimationController>();
    if (owner->animator->current_animation == animation->deathLeft) {
		owner->pModel = ResourceManager::Instance().getModel(65);
    }
    else {
        owner->pModel = ResourceManager::Instance().getModel(66);
    }

    //copy texture from animated rat
    owner->pModel->meshes[0].textures[0] = (newTexture);

    if (animation->currentState) {
        animation->currentState = nullptr;
    }
    animation->allFinished = true;
}

std::string DeathState::getName() const {
    return "DeathState";
}