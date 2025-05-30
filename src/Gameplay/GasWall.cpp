#include "GasWall.h"
#include "RegisterScript.h"
#include "../System/PhysicsSystem.h"
#include "../Component/BoundingBox.h"
#include "../Basic/Node.h"
#include "../config.h"
#include "GasParticle.h"
#include <iostream>
#include <cmath>
#include <random>
#include "../System/Tag.h"

REGISTER_SCRIPT(GasWall);

void GasWall::onAttach(Node* owner) {
    this->owner = owner;
    gasCreator = dynamic_cast<InstanceManager*>(owner);
}

void GasWall::onDetach() {
    owner = nullptr;
}

void GasWall::onStart() {
    obstacleLayer = { "Wall", "Floor" };
    glm::ivec2 startPos = glm::round(glm::vec2(owner->transform.getGlobalPosition().x, owner->transform.getGlobalPosition().y));
    spreadQueue.push(startPos);
    visited.insert(posKey(startPos));
    prefab = PrefabRegistry::FindPuzzleByName("GasParticle");
}

void GasWall::onUpdate(float deltaTime) {
    if (!spreading || spreadQueue.empty()) return;

    timer += deltaTime;
    if (timer >= spreadInterval * spreadSpeed) {
        spreadCloud();
        timer = 0.f;
    }
}

void GasWall::spreadCloud() {
    int count = std::min(16, static_cast<int>(spreadQueue.size()));

    const std::vector<glm::ivec2> directions = {
        { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 },
        { 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 }
    };

    while (count-- > 0 && !spreadQueue.empty()) {
        glm::ivec2 current = *(glm::ivec2*)&spreadQueue.front(); // tylko X i Y
        spreadQueue.pop();

        for (const auto& dir : directions) {
            glm::ivec2 newPos = current + dir;
            std::string key = posKey(newPos);

            if (visited.count(key)) continue;

            // SprawdŸ kolizje AABB (osie X i Y)
            bool isBlocked = false;
            for (Collider* box : PhysicsSystem::instance().getColliders() /*owner->scene_graph->root->getAllChildren()*/) {
				if (box->node == nullptr) continue;
				Node* node = box->node;
                if (std::find(obstacleLayer.begin(), obstacleLayer.end(), node->getLayerName()) != obstacleLayer.end()) {
                    glm::vec3 pos = node->transform.getGlobalPosition();
                    glm::vec3 scale = node->transform.getLocalScale();

                    float halfW = scale.x / 2.f;
                    float halfH = scale.y / 2.f;

                    if (newPos.x >= pos.x - halfW && newPos.x <= pos.x + halfW &&
                        newPos.y >= pos.y - halfH && newPos.y <= pos.y + halfH) {
                        isBlocked = true;
                        break;
                    }
                }
            }

            // SprawdŸ raycast
            //std::vector<Node*> nodes;
            //Ray ray = Ray{ glm::vec3(current, 0.f), glm::vec4(dir, 0.f, 0.f)};

            //bool isBlocked = false;
            //nodes.clear();
            //if (PhysicsSystem::instance().rayCast(ray, nodes, glm::distance(glm::vec2(current), glm::vec2(newPos)) * 0.75f)) {
            //    if (!(nodes.size() == 0)) {
            //        for (int i = 0; i < nodes.size(); i++) {
            //            isBlocked = std::ranges::any_of(obstacleLayer, [&](const std::string& l) {
            //                //std::cout << "Layer: " << nodes[i]->getLayerName() << ", is abstacle: " << (nodes[i]->getLayerName() == l) << std::endl;
            //                return nodes[i]->getLayerName() == l;
            //                });
            //        }
            //    }
            //}

            if (isBlocked) continue;

            visited.insert(key);
            spreadQueue.push(glm::ivec2(newPos));

            // Utwórz cz¹steczkê
            /*PrefabInstance* instance = new PrefabInstance(prefab, owner->scene_graph, "_" + std::to_string(counter++));
            Node* gas = instance->prefab_root->getChildById(0);

            gas->transform.setLocalScale(glm::vec3(0.f));
            gas->parent = owner;
            gas->transform.setLocalPosition(glm::vec3(newPos.x, newPos.y, 0.f) - owner->transform.getLocalPosition());
            gas->time_offset = 2.5f;*/
            glm::vec4 pos = glm::vec4(newPos.x, newPos.y, 0.f, 1.f); //- glm::vec4(owner->transform.getLocalPosition(), 1.f);
            
            //std::random_device rd;
            //std::mt19937 gen(rd()); // silnik
            //std::uniform_real_distribution<float> dist(0.0f, 3.1415f);

            //float randomValue = dist(gen);
            //
            //ParticleGasNode* gas = new ParticleGasNode(pos, randomValue);

            if (gasCreator) {
               /* std::shared_ptr<Layer> layer = TagLayerManager::Instance().getLayer("Gas");
                gas->setLayer(layer);*/
                ParticleGasStruct p = {pos, glfwGetTime()};

                gasCreator->addChild(p);
            }



            

        }
    }
}


void GasWall::onCollisionLogic(Node* other) {
    if (other->getTagName() == "Player") {
        spreading = true;
    }
}

std::string GasWall::posKey(const glm::ivec2& pos) const {
    return std::to_string(pos.x) + "_" + std::to_string(pos.y);
}

