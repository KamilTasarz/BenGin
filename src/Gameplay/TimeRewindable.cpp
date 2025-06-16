#include "TimeRewindable.h"
//#include "../Window/ServiceLocator.h"
#include "RegisterScript.h"
#include <GLFW/glfw3.h>
#include "GameManager.h"

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
        isRewinding = true;
    }
    else {
        isRewinding = false;
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
