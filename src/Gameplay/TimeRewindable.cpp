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

  //  if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_R) == GLFW_PRESS) {
  //      if (GameManager::instance->historyEmpty) {
  //          isRewinding = false;
  //      }
  //      else {
  //          isRewinding = true;

  //          rewindTime += deltaTime;
		//    rewindSpeed = pow(2, static_cast<int>(rewindTime) + 1);
  //      
  //          auto* audio = ServiceLocator::getAudioEngine();
  //          if (sfxId == -1) {
  //              sfxId = audio->PlayMusic(audio->rewind, GameManager::instance->sfxVolume * 60.f);
  //          }
  //      }
  //  }
  //  if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_R) == GLFW_RELEASE && isRewinding || !isRewinding) {
  //      auto* animationController = owner->getComponent<PlayerAnimationController>();
  //      //if (comp) comp->changeState(new IdleState());
  //      isRewinding = false;
		//rewindTime = 0.f;

  //      auto* audio = ServiceLocator::getAudioEngine();
  //      audio->stopSound(sfxId);
  //      sfxId = -1;
  //  }
    
    bool rewindKeyHeld = glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_R) == GLFW_PRESS;
    bool rewindKeyReleased = glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_R) == GLFW_RELEASE;

    // Klawisz puœci³ => zezwól na ponowne cofanie przy kolejnym naciœniêciu
    if (rewindKeyReleased) {
        hasReleasedRewindKey = true;
    }

    // Start rewind
    if (rewindKeyHeld && hasReleasedRewindKey && !GameManager::instance->historyEmpty) {
        isRewinding = true;
        rewindTime += deltaTime;
        rewindSpeed = pow(2, static_cast<int>(rewindTime) + 1);

        owner->scene_graph->activateRewindShader();

        auto* audio = ServiceLocator::getAudioEngine();
        if (sfxId == -1) {
            sfxId = audio->PlayMusic(audio->rewind, GameManager::instance->sfxVolume * 0.f);
        }
    }

    // Stop rewind if no history left
    if (isRewinding && GameManager::instance->historyEmpty) {
        isRewinding = false;
        rewindTime = 0.f;
        hasReleasedRewindKey = false;  // czeka na ponowne puszczenie klawisza

        owner->scene_graph->deactivateRewindShader();

        auto* audio = ServiceLocator::getAudioEngine();
        audio->pauseSound(sfxId);
        sfxId = -1;
    }

    // Stop rewind if key released manually
    if (rewindKeyReleased && isRewinding) {
        isRewinding = false;
        rewindTime = 0.f;
        hasReleasedRewindKey = true;

        owner->scene_graph->deactivateRewindShader();

        auto* audio = ServiceLocator::getAudioEngine();
        audio->pauseSound(sfxId);
        sfxId = -1;
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
