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
	//std::cout << colliders.size() << " " << colliders_RigidBody.size() << std::endl;
}

void PhysicsSystem::updateCollisions()
{

	std::set<std::pair<BoundingBox*, BoundingBox*>> testedPairs;

	int counter = 0;

	//zmienić na zrobienie listy coliderów z rigdibody i listy coliderów wszystkich (tez z rigidbody)
	// -> zmniejszenie kolizji + tylko rigidbody ma tak naprawde fizyke

	for (auto& collider1 : colliders_RigidBody) {
		
		for (auto& collider2 : colliders) {
			if (collider1->node == collider2->node) continue;

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
					
					if (comp->name._Equal("Rigidbody")) {
						first = !dynamic_cast<Rigidbody*>(comp.get())->is_static && !collider1->is_logic;
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
					
					if (comp->name._Equal("Rigidbody")) {
						second = !dynamic_cast<Rigidbody*>(comp.get())->is_static && !collider2->is_logic;
					}
				}

				if (first && !collider2->is_logic) {
					float sep = 1.f;
					if (second) {
						sep = 0.5f;
						collider2->separate(collider1, sep);
					}
					collider1->separate(collider2, sep);
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


	std::cout << "Sprawdzono: " << counter << std::endl;
}

bool PhysicsSystem::rayCast(Ray ray, std::vector<Node*>& collide_with, float length)
{
	if (length <= 0.f) {
		for (auto& collider : colliders) {
			if (collider->is_logic) continue;
			float t;
			if (collider->isRayIntersects(ray.direction, ray.origin, t)) {
				collide_with.push_back(collider->node);
			}
		}
	}
	else {
		for (auto& collider : colliders) {
			if (collider->is_logic) continue;
			float t;
			if (collider->isRayIntersects(ray.direction, ray.origin, t)) {
				
				glm::vec3 hitPoint = ray.origin + glm::vec3(ray.direction * t);
				float distance = glm::length(hitPoint - ray.origin);
				if (distance <= length) {
					collide_with.push_back(collider->node);
				}
			}
		}
	}
	return collide_with.size() > 0;
}

bool PhysicsSystem::rayCast(const std::vector<Ray>& rays, std::vector<Node*>& collide_with, float length)
{
	bool hitAny = false;

	for (const Ray& ray : rays) {
		bool hit = rayCast(ray, collide_with, length);
		hitAny = hitAny || hit;
	}

	return hitAny;
}
