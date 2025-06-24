#pragma once

#include "Script.h"

class Platform : public Script
{
public:

	float timer = 0.f;
	bool flipped = false;
	bool downPressed = false;
	
	Platform() = default;
	virtual ~Platform() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;

	bool isPadButtonPressed(int button);
	float getPadAxis(int button);

	void onUpdate(float deltaTime) override;
	void onStayCollisionLogic(Node* other);

	void onExitCollisionLogic(Node* other) override;
	void onStayCollision(Node* other) override;

};

