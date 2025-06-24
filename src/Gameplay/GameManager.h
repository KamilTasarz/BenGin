#pragma once

#include "Script.h"
#include "../Basic/Node.h"
#include <deque>

class LevelGenerator;
class GameManagerRewindable;
class UIManager;

class GameManager
{
private:
	GameManager() {}

public:
	//static GameManager* instance;

	GameManager(const GameManager&) = delete;
	GameManager& operator=(const GameManager&) = delete;
	GameManager(GameManager&&) = delete;
	GameManager& operator=(GameManager&&) = delete;



	float globalSmoothing = 10.f;
	float gasSpreadingSpeed = 1.f;
	Node* currentPlayer;
	Node* playerSpawner;
	LevelGenerator* levelGenerator;
	InstanceManager* emitter;
	UIManager* uiManager;

	std::deque<Node*> players;

	bool isRewinding = false;
	bool historyEmpty = true;

	// run variables
	float runTime = 0.f;
	int deathCount = 0;
	float score = 0.f;
	bool tutorialActive = false;

	float musicVolume = 1.f;
	float sfxVolume = 1.f;

	std::string player_name;
	bool isGamepadConnected = false;

	//glm::vec3 closestParticlePos = glm::vec3(0, 0, 0);
	float minPlayerToParticleDistance = 100.f;



	static GameManager& instance();
	//GameManager() = default;
	//~GameManager() = default;
	/*void onAttach(Node* owner) override;
	void onDetach() override;*/
	void Init(SceneGraph* scene_graph);
	void Update(float deltaTime, SceneGraph* scene_graph);
	void RemovePlayer();
	void RemoveCurrentPlayer();
	void RemoveThisPlayer(Node* player);
	void CalculateGasSpreadingSpeed(float deltaTime);
	void HandleLevelGeneration();
	void onEnd();
	void HandleRewindTimeline();
};

