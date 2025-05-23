#pragma once

#include "../config.h"

class BoundingBox;
class SceneGraph;
class Node;
struct Ray;

class PhysicsSystem
{
private:
    std::vector<BoundingBox*> colliders;
    std::vector<BoundingBox*> colliders_RigidBody;

    std::vector<BoundingBox*> colliders_logic;
    std::vector<BoundingBox*> colliders_RigidBody_logic;
public:
    static PhysicsSystem& instance();
    
	//zmienia vector koliderów, na te co są w frustum
    void updateColliders(SceneGraph *scene_graph);

    //sprawdza kolizje
    void updateCollisions();

    bool rayCast(Ray ray, std::vector<Node*>& collide_with, float length = -1.f, Node* owner = nullptr);
    bool rayCast(const std::vector<Ray>& rays, std::vector<Node*>& collide_with, float length = -1.f, Node* owner = nullptr);

	std::vector<BoundingBox*>& getColliders() {
		return colliders;
	}
};

