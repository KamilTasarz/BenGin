#include "PlayerRewindable.h"
#include "PlayerController.h"
#include "../System/Tag.h"
#include "RegisterScript.h"
#include "../Basic/Node.h"
#include "GameManager.h"
#include "GameManagerRewindable.h"
#include "RewindManager.h"

REGISTER_SCRIPT(PlayerRewindable);

void PlayerRewindable::onAttach(Node* owner) {
	TimeRewindable::onAttach(owner);
	this->owner = owner;
    RewindManager::Instance().registerRewindable(this);
}

void PlayerRewindable::onDetach() {
	TimeRewindable::onDetach();
	playerController = nullptr;
	owner = nullptr;
}

void PlayerRewindable::onStart() {
    playerController = owner->getComponent<PlayerController>();
}

void PlayerRewindable::onEnd()
{
}

std::shared_ptr<ITimeSnapshot> PlayerRewindable::createSnapshot() {
    auto snap = std::make_shared<PlayerSnapshot>();
    snap->isDead = playerController->isDead;
    snap->isGravityFlipped = playerController->isGravityFlipped;
	snap->isElectrified = playerController->isElectrified;
    snap->virusType = playerController->virusType;
    snap->tagName = owner->getTagName();
	snap->LayerName = owner->getLayerName();
    return snap;
}

void PlayerRewindable::applySnapshot(const std::shared_ptr<ITimeSnapshot>& baseSnap) {
    auto snap = std::dynamic_pointer_cast<PlayerSnapshot>(baseSnap);
    if (!snap) return;

    playerController->isDead = snap->isDead;
    playerController->isGravityFlipped = snap->isGravityFlipped;
	playerController->isElectrified = snap->isElectrified;
    playerController->virusType = snap->virusType;
    playerController->lastVirusType = "null";

    if (owner->getTagName() != snap->tagName) {
        auto tag = TagLayerManager::Instance().getTag(snap->tagName);
        owner->setTag(tag);
    }
	if (owner->getLayerName() != snap->LayerName) {
		auto layer = TagLayerManager::Instance().getLayer(snap->LayerName);
		owner->setLayer(layer);
	}

    if (history.empty() && GameManager::instance->rewindable->history.size() > 1) {
		GameManager::instance->RemoveCurrentPlayer();
    }
}
