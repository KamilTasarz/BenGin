#include "VirusRewindable.h"
#include "Puzzles/Virus.h"
#include "../System/Tag.h"
#include "RegisterScript.h"
#include "../Basic/Node.h"
#include "RewindManager.h"

REGISTER_SCRIPT(VirusRewindable);

void VirusRewindable::onAttach(Node* owner) {
	this->owner = owner;
    RewindManager::Instance().registerRewindable(this);
}

void VirusRewindable::onDetach() {
	virus = nullptr;
	owner = nullptr;
	RewindManager::Instance().unregisterRewindable(this);
}

void VirusRewindable::onStart() {
    virus = owner->getComponent<Virus>();
}

std::shared_ptr<ITimeSnapshot> VirusRewindable::createSnapshot() {
    auto snap = std::make_shared<VirusSnapshot>();

	snap->isCollected = virus->isCollected;
	snap->rotation = owner->transform.getLocalRotation();

    return snap;
}

void VirusRewindable::applySnapshot(const std::shared_ptr<ITimeSnapshot>& baseSnap) {
    auto snap = std::dynamic_pointer_cast<VirusSnapshot>(baseSnap);
    if (!snap) return;
    
	virus->isCollected = snap->isCollected;
	owner->transform.setLocalRotation(snap->rotation);
	virus->modelChanged = false;
}
