#pragma once

#include "Script.h"

class Checkpoint : public Script
{
public:
	Checkpoint() = default;
	~Checkpoint() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onCollisionLogic(Node* other) override;
};

