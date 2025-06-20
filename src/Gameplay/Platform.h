#pragma once

#include "Script.h"

class Platform : public Script
{
public:
	//using SelfType = Platform;
	float timer = 0.f;
	
	Platform() = default;
	virtual ~Platform() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	//void onStart() override;
	void onUpdate(float deltaTime) override;
	void onStayCollisionLogic(Node* other);
	//void onEnd() override;
	
	//void onCollisionLogic(Node* other) override;
	void onExitCollisionLogic(Node* other) override;
	void onStayCollision(Node* other) override;
};

