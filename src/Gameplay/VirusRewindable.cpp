#include "VirusRewindable.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "Puzzles/Virus.h"

REGISTER_SCRIPT(VirusRewindable);

void VirusRewindable::onUpdate(float deltaTime) {
    if (!owner) return;

    if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_R) == GLFW_PRESS) {
        isRewinding = true;
    }
    else isRewinding = false;

    if (isRewinding) {
        for (int i = 0; i < rewindSpeed; ++i) {
            if (!virusHistory.empty()) {
                VirusSnapshot snap = virusHistory.back();
                virusHistory.pop_back();

                owner->transform.setLocalPosition(snap.position);
                owner->transform.setLocalRotation(snap.rotation);

				virus->isCollected = snap.isCollected;
                virus->player = snap.player;
            }
        }
    }
    else {
        VirusSnapshot snap;
        snap.position = owner->transform.getLocalPosition();
        snap.rotation = owner->transform.getLocalRotation();

		snap.isCollected = virus->isCollected;
		snap.player = virus->player;

        virusHistory.push_back(snap);
    }
}
