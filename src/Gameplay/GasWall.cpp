#include "GasWall.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "GameMath.h"
#include "GasParticle.h"
#include "../System/PhysicsSystem.h"
#include "../config.h"
#include <iostream>
#include <string>

REGISTER_SCRIPT(GasWall);

void GasWall::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "GasParticle::onAttach::" << owner->name << std::endl;
}

void GasWall::onDetach()
{
	std::cout << "GasWall::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void GasWall::onStart()
{
	std::cout << "GasWall::onStart::" << owner->name << std::endl;

    spreadInterval = 1.f;
	obstacleLayer = { "Floor", "Wall"};
    glm::vec3 startPos = owner->transform.getLocalPosition();
    spreadQueue.push(startPos);
    //visited.insert(posKey(startPos));

    prefab = PrefabRegistry::FindByName("GasParticle");
    counter = 0;
}

void GasWall::onUpdate(float deltaTime) {
    timer += deltaTime;

    /*if (Input::isKeyPressed(KEY_P)) {
        spreading = !spreading;
    }*/

    /*std::cout << "timer: " << timer << std::endl;
	std::cout << "spreadInterval: " << spreadInterval * spreadSpeed << std::endl;
	std::cout << "spreading: " << spreading << std::endl;
	std::cout << "spreadQueue size: " << spreadQueue.size() << std::endl;*/

    if (timer >= spreadInterval * spreadSpeed && spreading && !spreadQueue.empty()) {
        spreadCloud(deltaTime);
        timer = 0.f;

        //gm->score += 10.f * gm->scoreMultiplier;
    }
}

void GasWall::spreadCloud(float deltaTime) {
    int count = spreadQueue.size();

	//float timeDelay = spreadInterval * spreadSpeed / count;
	//float delayTimer = 0.f;
    float time = glfwGetTime();
    for (int i = 0; i < count; ++i) {

		/*delayTimer = 0.f;
		while (delayTimer < timeDelay) {
			delayTimer += deltaTime;
		}*/

        glm::vec3 currentPos = spreadQueue.front();
        spreadQueue.pop();

        std::vector<glm::vec3> directions = {
            { 1, 0, 0 }, { -1, 0, 0 },
            { 0, 1, 0 }, { 0, -1, 0 },
            { 1, 1, 0 }, { 1, -1, 0 },
            { -1, 1, 0 }, { -1, -1, 0 }
        };
        
        for (const auto& dir : directions) {
            glm::vec3 newPos = currentPos + dir;
            std::string key = posKey(newPos);

            std::vector<Node*> nodes;
			Ray ray = Ray{ currentPos, glm::vec4(dir, 0.f) };

            bool obstacle = false;
            nodes.clear();
            if (PhysicsSystem::instance().rayCast(ray, nodes, glm::distance(currentPos, newPos) * 0.6f)) {
                if (!(nodes.size() == 0)) {
                    for (int i = 0; i < nodes.size(); i++) {
                        obstacle = std::ranges::any_of(obstacleLayer, [&](const std::string& l) {
							//std::cout << "Layer: " << nodes[i]->getLayerName() << ", is abstacle: " << (nodes[i]->getLayerName() == l) << std::endl;
                            return nodes[i]->getLayerName() == l;
                            });
					}
                }
            }

            if (visited.count(key) == 0 && !obstacle) {

                float chance = (dir.x != 0 && dir.y != 0) ? 0.3f : 1.f;

                if (GameMath::RandomFloat(0.f, 1.f) < chance) {
                    visited.insert(key);
                    spreadQueue.push(newPos);

                    PrefabInstance* pref = new PrefabInstance(prefab, owner->scene_graph, "_" + std::to_string(counter));
                    Node* gas = pref->prefab_root->getChildById(0);

                    gas->transform.setLocalScale(glm::vec3(0.f));
                    gas->parent = owner;
                    gas->transform.setLocalPosition(newPos - owner->transform.getLocalPosition());
                    gas->time_offset = 2.5f;
                    //GasParticle* particle = gas->getComponent<GasParticle>();
					//particle->growTime = spreadInterval * 2.5f;

                    owner->scene_graph->addChild(gas, owner);
                }
            }
        }
       
    }
    float time2 = glfwGetTime();
    std::cout << time2 - time << std::endl;
}

std::string GasWall::posKey(const glm::vec3& pos) {
    int x = static_cast<int>(round(pos.x * 100));
    int y = static_cast<int>(round(pos.y * 100));
    int z = static_cast<int>(round(pos.z * 100));
    return std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z);
}