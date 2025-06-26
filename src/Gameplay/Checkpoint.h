#pragma once

#include "Script.h"

class Checkpoint : public Script
{
public:
	//glm::vec3 minColliderPosition;
	//glm::vec3 maxColliderPosition;

	Node* arm = nullptr;
	Node* tank = nullptr;
	Node* filling = nullptr;

	float timer;
	bool fillingStarted = false;

	Checkpoint() = default;
	~Checkpoint() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onCollisionLogic(Node* other) override;
	void onStayCollisionLogic(Node* other);
};

