#include "PlayerRewindable.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "PlayerController.h"
#include "../System/Tag.h"
#include "GameManager.h"

REGISTER_SCRIPT(PlayerRewindable);

void PlayerRewindable::onUpdate(float deltaTime) {
    if (!owner) return;

    if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_R) == GLFW_PRESS) {
        isRewinding = true;
    }
    else isRewinding = false;

    if (isRewinding && !playerHistory.empty()) {
        for (int i = 0; i < rewindSpeed; ++i) {
            if (!playerHistory.empty()) {
                PlayerSnapshot snap = playerHistory.back();
                playerHistory.pop_back();

                owner->transform.setLocalPosition(snap.position);
                owner->transform.setLocalRotation(snap.rotation);

                playerController->isDead = snap.isDead;
                playerController->isGravityFlipped = snap.isGravityFlipped;
				playerController->virusType = snap.virusType;
				playerController->lastVirusType = "null";

				if (owner->getTagName() != snap.tagName) {
                    std::shared_ptr<Tag> tag = TagLayerManager::Instance().getTag(snap.tagName);
                    owner->setTag(tag);
				}
            }
            else {
				GameManager::instance->RemoveCurrentPlayer();
                return;
            }
        }
    }
    else {
        PlayerSnapshot snap;
        snap.position = owner->transform.getLocalPosition();
        snap.rotation = owner->transform.getLocalRotation();

		snap.isDead = playerController->isDead;
		snap.isGravityFlipped = playerController->isGravityFlipped;
		snap.virusType = playerController->virusType;
		snap.tagName = owner->getTagName();

        playerHistory.push_back(snap);
    }
}
