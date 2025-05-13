#include "PhysicsSystem.h"
#include "../Basic/Node.h"
#include "../Component/BoundingBox.h"
#include "../Component/CameraGlobals.h"
#include "../System/Component.h"
#include "../System/Rigidbody.h"

PhysicsSystem& PhysicsSystem::instance() {
    static PhysicsSystem instance;
    return instance;
}

void PhysicsSystem::updateColliders(SceneGraph* scene_graph)
{
	colliders.clear();
	colliders_RigidBody.clear();
	scene_graph->root->checkIfInFrustrum(colliders, colliders_RigidBody);
	
}

void PhysicsSystem::updateCollisions()
{

	std::set<std::pair<BoundingBox*, BoundingBox*>> testedPairs;

	//zmienić na zrobienie listy coliderów z rigdibody i listy coliderów wszystkich (tez z rigidbody)
	// -> zmniejszenie kolizji + tylko rigidbody ma tak naprawde fizyke

	for (auto& collider1 : colliders_RigidBody) {
		
		for (auto& collider2 : colliders) {
			if (collider1 == collider2) continue;

			BoundingBox* first = (collider1 < collider2) ? collider1 : collider2;
			BoundingBox* second = (collider1 < collider2) ? collider2 : collider1;

			std::pair<BoundingBox*, BoundingBox*> pair = { first, second };

			// pomiń, jeśli już było
			if (testedPairs.find(pair) != testedPairs.end()) continue;

			testedPairs.insert(pair);

			if (collider1->isBoundingBoxIntersects(*collider2)) {

				bool first = false, second = false;
				for (auto& comp : collider1->node->components) {
					comp->onCollision(collider2->node);
					if (comp->name._Equal("Rigidbody")) {
						first = !dynamic_cast<Rigidbody*>(comp.get())->is_static;
					}
				}
				for (auto& comp : collider2->node->components) {
					comp->onCollision(collider1->node);
					if (comp->name._Equal("Rigidbody")) {
						second = !dynamic_cast<Rigidbody*>(comp.get())->is_static;
					}
				}

				if (first) {
					float sep = 1.f;
					if (second) {
						sep = 0.5f;
						collider2->separate(collider1, sep);
					}
					collider1->separate(collider2, sep);
				}
				else if (second) {

					float sep = 1.f;
					if (first) {
						sep = 0.5f;
						collider1->separate(collider2, sep);
					}
					collider2->separate(collider1, sep);
				}
				
			}
		}
	}
}
