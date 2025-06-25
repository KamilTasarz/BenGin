#include "TimeRewindable.h"
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
#include "MusicManager.h"

bool TimeRewindable::isPadButtonPressed(int button) {

    if (!glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) return false;
    GLFWgamepadstate state;
    if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
        return state.buttons[button] == GLFW_PRESS;
    }
    return false;

}

bool TimeRewindable::isPadButtonReleased(int button) {

    if (!glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) return false;
    GLFWgamepadstate state;
    if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
        return state.buttons[button] == GLFW_RELEASE;
    }
    return false;

}


void TimeRewindable::onUpdate(float deltaTime) {
    if (!owner) return;

    glm::vec3 newCheckpointPos = GameManager::instance().playerSpawner->transform.getGlobalPosition();
    if (newCheckpointPos != lastCheckpointPos) {
        resetHistory();
        lastCheckpointPos = newCheckpointPos;
    }

    bool rewindKeyHeld = glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_R) == GLFW_PRESS || isPadButtonPressed(GLFW_GAMEPAD_BUTTON_B) || isPadButtonPressed(GLFW_GAMEPAD_BUTTON_Y);
    //bool rewindKeyReleased = glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_R) == GLFW_RELEASE && isPadButtonReleased(GLFW_GAMEPAD_BUTTON_B) && isPadButtonReleased(GLFW_GAMEPAD_BUTTON_Y);

    // Klawisz puœci³ => zezwól na ponowne cofanie przy kolejnym naciœniêciu
    if (!rewindKeyHeld) {
        hasReleasedRewindKey = true;
    }

    // Start rewind
    if (rewindKeyHeld && hasReleasedRewindKey && !GameManager::instance().historyEmpty) {
        isRewinding = true;
        rewindTime += deltaTime;
        rewindSpeed = pow(2, static_cast<int>(rewindTime) + 1);
        owner->scene_graph->activateRewindShader();

        //MusicManager::instance().PlayRewindSound();
    }


    // Stop rewind if no history left
    if (isRewinding && GameManager::instance().historyEmpty) {
        isRewinding = false;
        rewindTime = 0.f;
        hasReleasedRewindKey = false;
        owner->scene_graph->deactivateRewindShader();

        //MusicManager::instance().StopRewindSound();
    }

    // Stop rewind if key released manually
    if (!rewindKeyHeld && isRewinding) {
        isRewinding = false;
        rewindTime = 0.f;
        hasReleasedRewindKey = true;
        owner->scene_graph->deactivateRewindShader();

        //MusicManager::instance().StopRewindSound();
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
        //MusicManager::instance().StopRewindSound();
    }
}

void TimeRewindable::pushSnapshot(std::shared_ptr<ITimeSnapshot> snapshot) {
    history.push_back(snapshot);
}

void TimeRewindable::resetHistory() {
	history.clear();
    
}
