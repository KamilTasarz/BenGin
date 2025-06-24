#pragma once
#include "Script.h"

class ButtonObject;
class SpriteObject;

class AnimatedMenu : public Script
{
public:
	using SelfType = AnimatedMenu;
	
	ButtonObject* play = nullptr;
	ButtonObject* lead = nullptr;
	ButtonObject* exit = nullptr;
	ButtonObject* back = nullptr;
	SpriteObject* background = nullptr;
	std::string text;
	bool isActive = false;
	bool right = true;
	float timer = 0.f, buffer = 0.5f, dx = 0.f, max_dx = 1920.f, spd = 2.f;
	float play_x, lead_x, exit_x, back_x, alfa = 0.f;

	void onStart() override;
	void onUpdate(float deltaTime) override;
};

