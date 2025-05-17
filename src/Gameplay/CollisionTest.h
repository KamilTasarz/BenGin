#pragma once

#include "Script.h"

class CollisionTest : public Script
{
public:
	CollisionTest() = default;
	virtual ~CollisionTest() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	//void onStart() override;
	void onUpdate(float deltaTime) override;
	//void onEnd() override;
	void onCollision(Node* other) override;
	void onStayCollision(Node* other) override;
	void onExitCollision(Node* other) override;
	void onCollisionLogic(Node* other) override;
	void onStayCollisionLogic(Node* other) override;
	void onExitCollisionLogic(Node* other) override;
};

