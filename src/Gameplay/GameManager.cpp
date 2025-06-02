#include "GameManager.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "PlayerSpawner.h"
#include "LevelGenerator.h"

REGISTER_SCRIPT(GameManager);

GameManager* GameManager::instance = nullptr;

void GameManager::onAttach(Node* owner)
{
    this->owner = owner;
}

void GameManager::onDetach()
{
    owner = nullptr;
}

void GameManager::onStart()
{
    GameManager::instance = this;
    playerSpawner = owner->scene_graph->root->getChildByTag("PlayerSpawner");
    levelGenerator = owner->scene_graph->root->getChildByTag("LevelGenerator");
    emitter = dynamic_cast<InstanceManager*>(owner->scene_graph->root->getChildByTag("Emitter"));
}

void GameManager::onUpdate(float deltaTime)
{
    CalculateGasSpreadingSpeed(deltaTime);
    HandleLevelGeneration();
}

void GameManager::CalculateGasSpreadingSpeed(float deltaTime) {
    int index = emitter->tail - 1;

    glm::vec3 gasPos = emitter->particles[index].position;
    glm::vec3 spawnerPos = playerSpawner->transform.getGlobalPosition();

    float distanceFromGasToSpawner = glm::distance(gasPos, spawnerPos);

    gasSpreadingSpeed = 10.f / distanceFromGasToSpawner;
    gasSpreadingSpeed = glm::clamp(gasSpreadingSpeed, 0.3f, 3.f);

    std::cout << gasSpreadingSpeed << std::endl;
}

void GameManager::HandleLevelGeneration() {
    glm::vec3 playerPos = playerSpawner->getComponent<PlayerSpawner>()->currentPlayer->transform.getGlobalPosition();
    glm::vec3 generatorPos = levelGenerator->transform.getGlobalPosition();

    float distanceToLevelGen = glm::distance(playerPos, generatorPos);

    if (distanceToLevelGen < 30.f) {
        levelGenerator->getComponent<LevelGenerator>()->GenerateLevel();
    }
}