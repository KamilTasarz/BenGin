#include "TimeRewindable.h"
//#include "../Window/ServiceLocator.h"
#include "RegisterScript.h"
#include <GLFW/glfw3.h>
#include "GameManager.h"
#include "Animation/PlayerAnimationController.h"
#include "Animation/IdleState.h"
#include "../Basic/Animator.h"
#include "Animation/FallState.h"
#include "Animation/JumpState.h"
#include "Animation/RiseState.h"
#include "Animation/LandState.h"
#include "Animation/RunState.h"
#include "Animation/DeathState.h"
#include "Animation/TurnState.h"
#include "Animation/PushState.h"

//REGISTER_SCRIPT(TimeRewindable);
//
//void TimeRewindable::onAttach(Node* owner) {
//    this->owner = owner;
//}
//
//void TimeRewindable::onDetach() {
//    history.clear();
//    owner = nullptr;
//}

void TimeRewindable::onUpdate(float deltaTime) {
    if (!owner) return;

	glm::vec3 newCheckpointPos = GameManager::instance->playerSpawner->transform.getGlobalPosition();
	if (newCheckpointPos != lastCheckpointPos) {
		resetHistory();
		lastCheckpointPos = newCheckpointPos;
	}

    if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_R) == GLFW_PRESS) {
		rewindTime += deltaTime;

        isRewinding = true;
		rewindSpeed = pow(2, static_cast<int>(rewindTime) + 1);
    }
    if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_R) == GLFW_RELEASE && isRewinding) {
        auto* animationController = owner->getComponent<PlayerAnimationController>();
        //if (comp) comp->changeState(new IdleState());
        isRewinding = false;
		rewindTime = 0.f;

        /*if (owner->animator) {
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
        }*/
    }
    

    if (isRewinding) {
        for (int i = 0; i < rewindSpeed && !history.empty(); ++i) {
            auto snapshot = history.back();
            history.pop_back();
            applySnapshot(snapshot);
        }
    }
    else {
        pushSnapshot(createSnapshot());
    }
}

void TimeRewindable::pushSnapshot(std::shared_ptr<ITimeSnapshot> snapshot) {
    history.push_back(snapshot);

    // Optional: implement size/time limit
    // while (history.size() > limit) history.pop_front();
}

void TimeRewindable::resetHistory() {
	history.clear();
}
