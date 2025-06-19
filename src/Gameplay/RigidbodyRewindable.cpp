#include "RigidbodyRewindable.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "RewindManager.h"
#include "../System/Rigidbody.h"

REGISTER_SCRIPT(RigidbodyRewindable);

void RigidbodyRewindable::onAttach(Node* owner) {
	this->owner = owner;
    rigidbody = owner->getComponent<Rigidbody>();
    RewindManager::Instance().registerRewindable(this);
}

void RigidbodyRewindable::onDetach() {
    owner = nullptr;
	RewindManager::Instance().unregisterRewindable(this);
}

void RigidbodyRewindable::onStart() {
}

std::shared_ptr<ITimeSnapshot> RigidbodyRewindable::createSnapshot() {
    auto snap = std::make_shared<RigidbodySnapshot>();
    snap->position = owner->transform.getLocalPosition();
    snap->rotation = owner->transform.getLocalRotation();
	snap->velocityX = rigidbody->velocityX;
	snap->velocityY = rigidbody->velocityY;
	snap->is_static = rigidbody->is_static;
    return snap;
}

void RigidbodyRewindable::applySnapshot(const std::shared_ptr<ITimeSnapshot>& baseSnap) {
    auto snap = std::dynamic_pointer_cast<RigidbodySnapshot>(baseSnap);
    if (!snap) return;

    owner->transform.setLocalPosition(snap->position);
    owner->transform.setLocalRotation(snap->rotation);
	rigidbody->velocityX = snap->velocityX;
	rigidbody->velocityY = snap->velocityY;
	rigidbody->is_static = snap->is_static; 
}
