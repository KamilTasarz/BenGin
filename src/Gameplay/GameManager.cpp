#include "GameManager.h"
#include "../Basic/Node.h"
//#include "RegisterScript.h"
#include "PlayerSpawner.h"
#include "LevelGenerator.h"
#include "../System/GuiManager.h"
#include "GameManagerRewindable.h"
#include "PlayerRewindable.h"
#include "RewindManager.h"
#include "UIManager.h"

//REGISTER_SCRIPT(GameManager);

//GameManager* GameManager::instance = nullptr;

GameManager& GameManager::instance()
{
    static GameManager instance;
    return instance;
}

//void GameManager::onAttach(Node* owner)
//{
//	//GameManager::instance = this;
//    this->owner = owner;
//}
//
//void GameManager::onDetach()
//{
//    owner = nullptr;
//}

void GameManager::Init(SceneGraph* scene_graph)
{
    playerSpawner = scene_graph->root->getChildByTag("PlayerSpawner");
    if (scene_graph->root->getChildByTag("LevelGenerator")) {
        levelGenerator = scene_graph->root->getChildByTag("LevelGenerator")->getComponent<LevelGenerator>();
    }

    emitter = dynamic_cast<InstanceManager*>(scene_graph->root->getChildByTag("Emitter"));
    if (scene_graph->root->getChildByTag("UIManager")) {
       uiManager = scene_graph->root->getChildByTag("UIManager")->getComponent<UIManager>();
    }

	runTime = 0.f;
	score = 0.f;
	deathCount = 0;
}

void GameManager::Update(float deltaTime, SceneGraph* scene_graph)
{
	//isRewinding = uiManager->isRewinding;
	//historyEmpty = uiManager->rewindable->history.empty();
	if (!emitter) return;

    if (!tutorialActive && !isRewinding) {
        runTime += deltaTime;
        score += deltaTime * 10.f / gasSpreadingSpeed;
    }

    if (players.size() > 12) {
        RemovePlayer();
    }

    CalculateGasSpreadingSpeed(deltaTime);
    HandleLevelGeneration();
}

void GameManager::RemovePlayer()
{
	if (players.empty()) return;

	Node* playerToRemove = players.front();
	playerToRemove->scene_graph->deleteChild(playerToRemove);
    players.pop_front();
}

void GameManager::RemoveCurrentPlayer()
{
    if (players.size() <= 1) return;
    Node* playerToRemove = players.back();

    auto* player = playerToRemove->getComponent<PlayerRewindable>();
    RewindManager::Instance().unregisterRewindable(player);

    playerToRemove->scene_graph->deleteChild(playerToRemove);
    players.pop_back();

    if (!players.empty()) currentPlayer = players.back();
}

void GameManager::RemoveThisPlayer(Node* player)
{
	if (!player || players.empty()) return;

    player->scene_graph->deleteChild(player);

    auto playerNode = std::find(players.begin(), players.end(), player);
    if (playerNode != players.end()) {
        players.erase(playerNode);
    }

	if (players.empty()) {
		currentPlayer = nullptr;
	}
	else {
		currentPlayer = players.back();
	}
}

void GameManager::CalculateGasSpreadingSpeed(float deltaTime) {
	if (!emitter || !currentPlayer) return;

    int index = emitter->tail - 1;

    glm::vec3 spawnerPos = playerSpawner->transform.getGlobalPosition();
    glm::vec3 playerPos = currentPlayer->transform.getGlobalPosition();

    glm::vec3 gasPos = emitter->particles[index].position;
    float distanceFromGasToSpawner = glm::distance(gasPos, spawnerPos);

    float minDistance = 100.f;
	int size = clamp(index, 0, 15);

	for (int i = 0; i < size; i++) {
		glm::vec3 particlePos = emitter->particles[index - i].position;
		float distance = glm::distance(particlePos, spawnerPos);
		if (i == 0 || distance < minDistance) {
			minDistance = distance;
		}
        distance = glm::distance(particlePos, playerPos);
        if (i == 0 || distance < minPlayerToParticleDistance) {
            minPlayerToParticleDistance = distance;
        }
	}

    gasSpreadingSpeed = 10.f / minDistance;

	if (tutorialActive) {
        gasSpreadingSpeed = glm::clamp(gasSpreadingSpeed, 0.3f, 200.f);
	}
    else {
        gasSpreadingSpeed = glm::clamp(gasSpreadingSpeed, 0.3f, 3.f);
        float modifier = 1.f + runTime / 300.f;
		gasSpreadingSpeed /= modifier;
    }

    std::cout << gasSpreadingSpeed << std::endl;
}

void GameManager::HandleLevelGeneration() {
    if (!currentPlayer) return;
    glm::vec3 playerPos = currentPlayer->transform.getGlobalPosition();
    glm::vec3 generatorPos = levelGenerator->getOwner()->transform.getGlobalPosition();

    float distanceToLevelGen = glm::distance(playerPos, generatorPos);

    if (distanceToLevelGen < 40.f) {
        levelGenerator->GenerateLevel();
    }
}

void GameManager::onEnd()
{
	if (!currentPlayer) return;
    for (Node* player : players) {
        if (player && player->scene_graph) {
            player->scene_graph->deleteChild(player);
        }
    }
    players.clear();
    currentPlayer = nullptr;
}