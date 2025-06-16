#include "GameManagerRewindable.h"
#include "../Basic/Node.h"
#include "GameManager.h"
#include "RegisterScript.h"
#include "RewindManager.h"

REGISTER_SCRIPT(GameManagerRewindable);

void GameManagerRewindable::onAttach(Node* owner) {
	this->owner = owner;
    RewindManager::Instance().registerRewindable(this);
}

void GameManagerRewindable::onDetach() {
	gameManager = nullptr;
	owner = nullptr;
	RewindManager::Instance().unregisterRewindable(this);
}

void GameManagerRewindable::onStart() {
    gameManager = owner->getComponent<GameManager>();
}

std::shared_ptr<ITimeSnapshot> GameManagerRewindable::createSnapshot() {
    auto snap = std::make_shared<GameManagerSnapshot>();
    snap->runTime = gameManager->runTime;
    snap->deathCount = gameManager->deathCount;
    snap->score = gameManager->score;
    return snap;
}

void GameManagerRewindable::applySnapshot(const std::shared_ptr<ITimeSnapshot>& baseSnap) {
    auto snap = std::dynamic_pointer_cast<GameManagerSnapshot>(baseSnap);
    if (!snap || !gameManager) return;

    gameManager->runTime = snap->runTime;
    gameManager->deathCount = snap->deathCount;
    gameManager->score = snap->score;
}
