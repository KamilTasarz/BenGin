#pragma once

#include "Script.h"
#include "../Basic/Node.h"
#include <deque>

class TextObject;
class LevelGenerator;

class GameManager : public Script
{
public:
	static GameManager* instance;

	float globalSmoothing = 10.f;
	float gasSpreadingSpeed = 1.f;
	Node* currentPlayer;
	Node* playerSpawner;
	LevelGenerator* levelGenerator;
	InstanceManager* emitter;
	std::deque<Node*> players;

	// run variables
	float runTime = 0.f;
	int deathCount = 0;
	float score = 0.f;
	bool tutorialActive = false;

	TextObject* scoreText;
	TextObject* deathCountText;
	TextObject* runTimeText;
	TextObject* fpsText;

	float fpsAccumulator = 0.f;
	int frameCount = 0;
	float timeSinceLastUpdate = 0.f;
	const float updateInterval = 0.5f;

	float musicVolume = 1.f;
	float sfxVolume = 1.f;

	GameManager() = default;
	~GameManager() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void RemovePlayer();
	void RemoveCurrentPlayer();
	void CalculateGasSpreadingSpeed(float deltaTime);
	void HandleLevelGeneration();
};

