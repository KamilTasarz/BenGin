#pragma once

#include "../config.h"

class BoundingBox;
class SceneGraph;

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
};

