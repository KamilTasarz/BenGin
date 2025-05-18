#pragma once

#include "Script.h"

class GroundObject : public Script
{
public:
	bool justGrounded = false;

	GroundObject() = default;
	virtual ~GroundObject() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	//void onStart() override;
	void onUpdate(float deltaTime) override;
	//void onEnd() override;
	void onStayCollision(Node* other) override;
	void onExitCollision(Node* other) override;
};

