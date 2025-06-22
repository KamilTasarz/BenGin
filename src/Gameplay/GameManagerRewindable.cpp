#include "GameManagerRewindable.h"
#include "../Basic/Node.h"
#include "GameManager.h"
#include "RegisterScript.h"
#include "RewindManager.h"

REGISTER_SCRIPT(GameManagerRewindable);

void GameManagerRewindable::onAttach(Node* owner) {
	this->owner = owner;
}

void GameManagerRewindable::onDetach() {
	//gameManager = nullptr;
	owner = nullptr;
}

void GameManagerRewindable::onStart() {
    //gameManager = owner->getComponent<GameManager>();
}

std::shared_ptr<ITimeSnapshot> GameManagerRewindable::createSnapshot() {
    auto snap = std::make_shared<GameManagerSnapshot>();
    snap->runTime = GameManager::instance().runTime;
    snap->deathCount = GameManager::instance().deathCount;
    snap->score = GameManager::instance().score;
    return snap;
}

void GameManagerRewindable::applySnapshot(const std::shared_ptr<ITimeSnapshot>& baseSnap) {
    auto snap = std::dynamic_pointer_cast<GameManagerSnapshot>(baseSnap);
    if (!snap) return;

    GameManager::instance().runTime = snap->runTime;
    GameManager::instance().deathCount = snap->deathCount;
    GameManager::instance().score = snap->score;
}
