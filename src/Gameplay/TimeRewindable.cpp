#include "TimeRewindable.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"

REGISTER_SCRIPT(TimeRewindable);

void TimeRewindable::onAttach(Node* owner)
{
	this->owner = owner;
}

void TimeRewindable::onDetach()
{
	history.clear();
	owner = nullptr;
}

void TimeRewindable::onUpdate(float deltaTime) {
    if (!owner) return;
    
    if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_R) == GLFW_PRESS) {
        isRewinding = true;
    }
    else isRewinding = false;

    if (isRewinding) {
        for (int i = 0; i < rewindSpeed; ++i) {
            if (!history.empty()) {
                TimeSnapshot snap = history.back();
                history.pop_back();
                owner->transform.setLocalPosition(snap.position);
                owner->transform.setLocalRotation(snap.rotation);
            }
        }
    }
    else {
        TimeSnapshot snap;
        snap.position = owner->transform.getLocalPosition();
        snap.rotation = owner->transform.getLocalRotation();

        pushSnapshot(snap);
    }
}

void TimeRewindable::pushSnapshot(const TimeSnapshot& snapshot) {
    history.push_back(snapshot);
    // zarz¹dzaj limitem czasu
    //float timePerSnapshot = 1.f / 60.f; // np. 60 FPS
    //accumulatedTime += timePerSnapshot;
    //while (accumulatedTime > maxTime && !history.empty()) {
    //    history.pop_front();
    //    accumulatedTime -= timePerSnapshot;
    //}
}

bool TimeRewindable::popSnapshot(TimeSnapshot& outSnapshot) {
    if (history.empty()) return false;
    outSnapshot = history.back();
    history.pop_back();
    return true;
}
