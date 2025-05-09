#include "PhysicsSystem.h"
#include "../Basic/Node.h"
#include "../Component/BoundingBox.h"
#include "../Component/CameraGlobals.h"
#include "../System/Component.h"

PhysicsSystem& PhysicsSystem::instance() {
    static PhysicsSystem instance;
    return instance;
}

void PhysicsSystem::updateColliders(SceneGraph* scene_graph)
{
	colliders.clear();
	scene_graph->root->checkIfInFrustrum(colliders);
	
}

void PhysicsSystem::updateCollisions()
{

	std::set<std::pair<BoundingBox*, BoundingBox*>> testedPairs;

	for (auto& collider1 : colliders) {
		for (auto& collider2 : colliders) {
			if (collider1 == collider2) continue;

			BoundingBox* first = (collider1 < collider2) ? collider1 : collider2;
			BoundingBox* second = (collider1 < collider2) ? collider2 : collider1;

			std::pair<BoundingBox*, BoundingBox*> pair = { first, second };

			// pomiń, jeśli już było
			if (testedPairs.find(pair) != testedPairs.end()) continue;

			testedPairs.insert(pair);

			if (collider1->isBoundingBoxIntersects(*collider2)) {
				collider1->separate(collider2);

				for (auto& comp : collider1->node->components)
					comp->onCollision(collider2->node);
				for (auto& comp : collider2->node->components)
					comp->onCollision(collider1->node);
			}
		}
	}
}
