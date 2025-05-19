#include "GasWall.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "GameMath.h"
#include "GasParticle.h"

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

    glm::vec3 startPos = owner->transform.getLocalPosition();
    spreadQueue.push(startPos);
    //visited.insert(posKey(startPos));
}

void GasWall::onUpdate(float deltaTime) {
    timer += deltaTime;

    /*if (Input::isKeyPressed(KEY_P)) {
        spreading = !spreading;
    }*/

    std::cout << "timer: " << timer << std::endl;
	std::cout << "spreadInterval: " << spreadInterval * spreadSpeed << std::endl;
	std::cout << "spreading: " << spreading << std::endl;
	std::cout << "spreadQueue size: " << spreadQueue.size() << std::endl;

    if (timer >= spreadInterval * spreadSpeed && spreading && !spreadQueue.empty()) {
        spreadCloud();
        timer = 0.f;

        //gm->score += 10.f * gm->scoreMultiplier;
    }
}

void GasWall::spreadCloud() {
    int count = spreadQueue.size();

    for (int i = 0; i < count; ++i) {
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

            if (visited.count(key) == 0 /*&&
                !PhysicsSystem::checkSphere(newPos, 0.5f, obstacleLayer)*/) {

                float chance = (dir.x != 0 && dir.y != 0) ? 0.3f : 1.f;

                if (GameMath::RandomFloat(0.f, 1.f) < chance) {
                    visited.insert(key);
                    spreadQueue.push(newPos);

                    PrefabInstance* pref = new PrefabInstance(PrefabRegistry::FindByName("GasParticle"), owner->scene_graph, newPos);
                    Node* gas = pref->prefab_root->getChildById(0);
                    gas->transform.setLocalPosition(newPos);
					gas->parent = owner;

                    GasParticle* particle = gas->getComponent<GasParticle>();
					particle->growTime = spreadInterval * 2.5f;

                    owner->scene_graph->addChild(gas);
                }
            }
        }
    }
}

std::string GasWall::posKey(const glm::vec3& pos) {
    int x = static_cast<int>(round(pos.x * 100));
    int y = static_cast<int>(round(pos.y * 100));
    int z = static_cast<int>(round(pos.z * 100));
    return std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z);
}