#pragma once

#include "Script.h"

class Spikes : public Script
{
public:
	Spikes() = default;
	virtual ~Spikes() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	//void onStart() override;
	//void onUpdate(float deltaTime) override;
	//void onEnd() override;
	
	void onCollision(Node* other) override;
};

