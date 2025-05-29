#pragma once

#include "Script.h"

class GameManager : public Script
{
public:
	static GameManager* instance;

	float globalSmoothing = 10.f;

	GameManager() = default;
	~GameManager() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
};

