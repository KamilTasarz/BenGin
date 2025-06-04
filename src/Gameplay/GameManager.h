#pragma once

#include "Script.h"
#include "../Basic/Node.h"

class GameManager : public Script
{
public:
	static GameManager* instance;

	float globalSmoothing = 10.f;
	float gasSpreadingSpeed = 1.f;
	Node* playerSpawner;
	Node* levelGenerator;
	InstanceManager* emitter;

	// run variables
	float runTime = 0.f;
	int deathCount = 0;
	float score = 0.f;

	GameManager() = default;
	~GameManager() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void CalculateGasSpreadingSpeed(float deltaTime);
	void HandleLevelGeneration();
};

