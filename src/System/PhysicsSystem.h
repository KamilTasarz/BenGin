#pragma once

#include "../config.h"

class BoundingBox;
class SceneGraph;
class Node;
struct Ray;

class PhysicsSystem
{
public:
    std::unordered_set<BoundingBox*> colliders;
    std::unordered_set<BoundingBox*> colliders_RigidBody;
    std::unordered_set<Node*> rooms;

    //std::vector<BoundingBox*> colliders_logic;
    //std::vector<BoundingBox*> colliders_RigidBody_logic;

    static PhysicsSystem& instance();
    
	//zmienia vector koliderów, na te co są w frustum
    void updateColliders(SceneGraph *scene_graph);

    //sprawdza kolizje
    void updateCollisions();

    bool rayCast(Ray ray, std::vector<Node*>& collide_with, float length = -1.f, Node* owner = nullptr);
    bool rayCast(const std::vector<Ray>& rays, std::vector<Node*>& collide_with, float length = -1.f, Node* owner = nullptr);

	std::unordered_set<BoundingBox*>& getColliders() {
		return colliders;
	}
};

