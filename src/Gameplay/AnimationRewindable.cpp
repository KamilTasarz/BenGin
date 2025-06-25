#include "AnimationRewindable.h"
#include "../Basic/Node.h"
#include "../Basic/Model.h"
#include "Animation/PlayerAnimationController.h"
#include "../Basic/Animator.h"
#include "RegisterScript.h"
#include "RewindManager.h"
#include "GameManager.h"
#include "GameManagerRewindable.h"
#include "Animation/IdleState.h"
#include "Animation/DeathState.h"
#include "Animation/RunState.h"
#include "Animation/LandState.h"
#include "Animation/JumpState.h"
#include "Animation/FallState.h"
#include "Animation/PushState.h"
#include "Animation/RiseState.h"
#include "Animation/TurnState.h"
#include "../ResourceManager.h"

REGISTER_SCRIPT(AnimationRewindable);

void AnimationRewindable::onAttach(Node* owner) {
	this->owner = owner;
    RewindManager::Instance().registerRewindable(this);
}

void AnimationRewindable::onDetach() {
	animationController = nullptr;
	owner = nullptr;
	RewindManager::Instance().unregisterRewindable(this);
}

void AnimationRewindable::onStart() {
    animationController = owner->getComponent<PlayerAnimationController>();
}

std::shared_ptr<ITimeSnapshot> AnimationRewindable::createSnapshot() {
    auto snap = std::make_shared<AnimationSnapshot>();
    snap->isTurning = animationController->isTurning;
    snap->targetRotation = animationController->targetRotation;
    snap->gravityFlipped = animationController->gravityFlipped;
    snap->facingRight = animationController->facingRight;
	snap->allFinished = animationController->allFinished;
	snap->isDead = animationController->player->isDead;
    shared_ptr<Model> model = animationController->getOwner()->pModel;
    snap->model_id = model->id;
    if (animationController->getOwner()->pModel->has_animations) {
        snap->currentAnimation = animationController->getOwner()->animator->current_animation->name;
        snap->time_animation = animationController->getOwner()->animator->current_time;
    }
    else {
		snap->currentAnimation = "null"; // No animation available
		snap->time_animation = 0.f; // Reset time if no animation
    }
    return snap;
}

void AnimationRewindable::applySnapshot(const std::shared_ptr<ITimeSnapshot>& baseSnap) {
    auto snap = std::dynamic_pointer_cast<AnimationSnapshot>(baseSnap);
    if (!snap) return;
	animationController->allFinished = snap->allFinished;
    int id = snap->model_id;
    animationController->getOwner()->pModel = ResourceManager::Instance().getModel(id);
    //if (id == 16) animationController->allFinished = false;
    if (!snap->allFinished) {

        //if (animationController->currentState == nullptr)
            //animationController->changeState(new IdleState());
        //animationController->player->isDead = snap->isDead;

        animationController->isTurning = snap->isTurning;
        animationController->targetRotation = snap->targetRotation;
        animationController->gravityFlipped = snap->gravityFlipped;
        animationController->facingRight = snap->facingRight;

        //if (animationController->getOwner()->pModel != ResourceManager::Instance().getModel(16)) {

        animationController->getOwner()->pModel = ResourceManager::Instance().getModel(id);
        /*if (snap->isDead) {
            animationController->changeState(new DeathState());
            return;
        }*/
        
        //}
        animationController->getOwner()->animator->current_animation = owner->pModel->getAnimationByName(snap->currentAnimation);
        animationController->getOwner()->animator->current_time = snap->time_animation;
		
        if (owner->animator) {
            std::string name_anim = owner->animator->current_animation->name;
            if (name_anim == "Idle") {
                animationController->changeState(new IdleState());
            }
            else if (name_anim == "Run") {
                animationController->changeState(new RunState());
            }
            else if (name_anim == "PushFinal") {
                animationController->changeState(new PushState());
            }
            else if (name_anim == "JumpFall") {
                animationController->changeState(new FallState());
            }
            else if (name_anim == "JumpUp") {
                animationController->changeState(new RiseState());
            }
            else if (name_anim == "JumpEnd") {
                animationController->changeState(new LandState());
            }
            else if (name_anim == "Turn") {
                animationController->changeState(new TurnState());
            }
            else if (name_anim == "DeadBackTailLeft" || name_anim == "DeadBackTailRight") {
                animationController->changeState(new DeathState());
            }
        }

    }

}
