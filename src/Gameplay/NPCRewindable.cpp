#include "NPCRewindable.h"
#include "RegisterScript.h"
#include "../Basic/Node.h"

REGISTER_SCRIPT(NPCRewindable);

void NPCRewindable::onAttach(Node* owner)
{
	this->owner = owner;
}

void NPCRewindable::onDetach()
{
	tv = nullptr;
	owner = nullptr;
}

void NPCRewindable::onStart() {
	tv = owner->getComponent<TVManager>();
}

std::shared_ptr<ITimeSnapshot> NPCRewindable::createSnapshot() {
    auto snap = std::make_shared<NPCSnapshot>();

    snap->isFleeing = tv->isFleeing;
    snap->isCatched = tv->tvAI->isCatched;
    snap->isActive = tv->tvAI->isActive;
	snap->isPhysic = tv->tvAI->getOwner()->is_physic_active;
    snap->targetVelocity = tv->targetVelocity;
    snap->currentVelocity = tv->currentVelocity;

    return snap;
}

void NPCRewindable::applySnapshot(const std::shared_ptr<ITimeSnapshot>& baseSnap) {
    auto snap = std::dynamic_pointer_cast<NPCSnapshot>(baseSnap);
    if (!snap) return;

	tv->isFleeing = snap->isFleeing;
	tv->tvAI->isCatched = snap->isCatched;
	tv->tvAI->isActive = snap->isActive;
	tv->tvAI->getOwner()->setPhysic(snap->isPhysic);
	tv->targetVelocity = snap->targetVelocity;
	tv->currentVelocity = snap->currentVelocity;
	snap.reset();
}
