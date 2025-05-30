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

	scene_graph->root->checkIfInFrustrum(colliders, colliders_RigidBody, rooms);
	//std::cout << colliders.size() << " " << colliders_RigidBody.size() << std::endl;
}

void PhysicsSystem::updateCollisions()
{

	std::set<std::pair<BoundingBox*, BoundingBox*>> testedPairs;

	int counter = 0;

	//zmienić na zrobienie listy coliderów z rigdibody i listy coliderów wszystkich (tez z rigidbody)
	// -> zmniejszenie kolizji + tylko rigidbody ma tak naprawde fizyke

	for (Collider* collider_rb : colliders_RigidBody) {
		
		BoundingBox* collider1 = dynamic_cast<BoundingBox*>(collider_rb);
		if (!collider1) continue;

		for (auto& collider : colliders) {

			BoundingBox* collider2 = dynamic_cast<BoundingBox*>(collider);

			if (!collider2) continue;

			if (collider1->node == collider2->node) continue;

			if (!collider1->active) {
				break;
			}
			if (!collider2->active) {
				continue; 
			}

			BoundingBox* first = (collider1 < collider2) ? collider1 : collider2;
			BoundingBox* second = (collider1 < collider2) ? collider2 : collider1;

			std::pair<BoundingBox*, BoundingBox*> pair = { first, second };
			// pomiń, jeśli już było
			if (testedPairs.find(pair) != testedPairs.end()) continue;

			testedPairs.insert(pair);

			counter++;

			if (collider1->isBoundingBoxIntersects(*collider2)) {

				if (collider1->current_collisons.find(collider2) == collider1->current_collisons.end()) {
					for (auto& comp : collider1->node->components) {
						if (!collider1->is_logic) 
						{
							comp->onCollision(collider2->node);
						}
						else {
							comp->onCollisionLogic(collider2->node);
						}
					}
					for (auto& comp : collider2->node->components) {
						if (!collider2->is_logic)
						{
							comp->onCollision(collider1->node);
						}
						else {
							comp->onCollisionLogic(collider1->node);
						}
					}
					collider1->current_collisons.insert(collider2);
					collider2->current_collisons.insert(collider1);
				}

				bool first = false, second = false;
				for (auto& comp : collider1->node->components) {
					if (!collider1->is_logic)
					{
						comp->onStayCollision(collider2->node);
					}
					else {
						comp->onStayCollisionLogic(collider2->node);
					}
				}
				for (auto& comp : collider2->node->components) {
					if (!collider2->is_logic)
					{
						comp->onStayCollision(collider1->node);
					}
					else {
						comp->onStayCollisionLogic(collider1->node);
					}
				}



				/*else if (second) {

					float sep = 1.f;
					if (first) {
						sep = 0.5f;
						collider1->separate(collider2, sep);
					}
					collider2->separate(collider1, sep);
				}*/
				
			}
			else {
				if (collider1->current_collisons.find(collider2) != collider1->current_collisons.end()) {
					collider1->current_collisons.erase(collider2);
					collider2->current_collisons.erase(collider1);
					for (auto& comp : collider1->node->components) {
						if (!collider1->is_logic)
						{
							comp->onExitCollision(collider2->node);
						}
						else {
							comp->onExitCollisionLogic(collider2->node);
						}
					}
					for (auto& comp : collider2->node->components) {
						
						if (!collider2->is_logic)
						{
							comp->onExitCollision(collider1->node);
						}
						else {
							comp->onExitCollisionLogic(collider1->node);
						}
					}
				}
			}
		}
	}


	//std::cout << "Sprawdzono: " << counter << std::endl;
}

bool PhysicsSystem::rayCast(Ray ray, std::vector<RayCastHit>& collide_with, float length, Node* owner)
{
	if (length <= 0.f) {
		for (Collider* collider : colliders) {

			//if (dynamic_cast<BoundingBox*>(collider)) continue;

			//BoundingBox* collider = dynamic_cast<BoundingBox*>(collider_base);

			if (collider->is_logic || !collider->active) continue;
			float t;
			glm::vec3 end;
			if (collider->isRayIntersects(ray.direction, ray.origin, t, end) && collider->node != owner) {
				RayCastHit r = { collider->node, end, t, dynamic_cast<BoundingBox*>(collider) ? true : false };
				collide_with.push_back(r);
			}

		}
	}
	else {
		for (Collider* collider : colliders) {

			//BoundingBox* collider = dynamic_cast<BoundingBox*>(collider_base);

			

			if (collider->is_logic || !collider->active) continue;
			float t;
			glm::vec3 end;
			if (collider->isRayIntersects(ray.direction, ray.origin, t, end) && collider->node != owner) {
				
				
				float distance = glm::length(end - ray.origin);
				if (distance <= length) {
					RayCastHit r = { collider->node, end, t, dynamic_cast<BoundingBox*>(collider) ? true : false };
					collide_with.push_back(r);
				}
			}
		}
	}

	std::sort(collide_with.begin(), collide_with.end(), [&](const RayCastHit& a, const RayCastHit& b) {
		return a.t < b.t;
		
		});

	return collide_with.size() > 0;
}

bool PhysicsSystem::rayCast(const std::vector<Ray>& rays, std::vector<RayCastHit>& collide_with, float length, Node* owner)
{
	bool hitAny = false;

	for (const Ray& ray : rays) {
		bool hit = rayCast(ray, collide_with, length, owner);
		hitAny = hitAny || hit;
	}

	return hitAny;
}
