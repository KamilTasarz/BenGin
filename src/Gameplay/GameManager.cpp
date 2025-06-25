#include "GameManager.h"
#include "../Basic/Node.h"
#include "PlayerSpawner.h"
#include "LevelGenerator.h"
#include "../System/GuiManager.h"
#include "../System/SceneManager.h"
#include "GameManagerRewindable.h"
#include "PlayerRewindable.h"
#include "RewindManager.h"
#include "UIManager.h"
#include "../config.h"
#include <fstream>
#include "MusicManager.h"

GameManager& GameManager::instance()
{
    static GameManager instance;
    return instance;
}

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
	game_over = false;
	runTime = 0.f;
	score = 0.f;
	deathCount = 0;
    added_stats = false;
    json stats;
	std::ifstream file("res/leaderboard.json");
	if (file.is_open()) {
		file >> stats;
		file.close();
        playerStats.clear();
        for (json row : stats) {
			Player_stats playerStat;
			playerStat.name = row["name"].get<std::string>();
			playerStat.score = row["score"].get<int>();
			
			playerStats.push_back(playerStat);
        }
		std::sort(playerStats.begin(), playerStats.end(), [](const Player_stats& a, const Player_stats& b) {
			return a.score > b.score;
			});
	}
}

void GameManager::Update(float deltaTime, SceneGraph* scene_graph)
{
	
	if (!emitter) return;

    if (game_over && !added_stats) {
		added_stats = true;
		Player_stats playerStat;
		playerStat.name = player_name;
		playerStat.score = (int) score;
		playerStats.push_back(playerStat);
		std::sort(playerStats.begin(), playerStats.end(), [](const Player_stats& a, const Player_stats& b) {
			return a.score > b.score;
			});
		// Save to file
		json stats;
		for (const auto& stat : playerStats) {
			stats.push_back({ {"name", stat.name.empty() ? "GUEST" : stat.name}, {"score", (int)stat.score}});
		}
		std::ofstream outFile("res/leaderboard.json");
		outFile << stats.dump(4);
		outFile.close();
        SceneManager::Instance().next();
    }

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

    static bool prevRewind = false;
    bool currentRewind = isRewinding;

    if (currentRewind && !prevRewind) {
        MusicManager::instance().PlayRewindSound();
    }
    else if (!currentRewind && prevRewind) {
        MusicManager::instance().StopRewindSound();
    }

    prevRewind = currentRewind;

    //std::cout << gasSpreadingSpeed << std::endl;
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