#pragma once
#include "Script.h"

class ButtonObject;
class SpriteObject;
class TextObject;

class AnimatedMenu : public Script
{
public:
	using SelfType = AnimatedMenu;
	
	ButtonObject* play = nullptr;
	ButtonObject* lead = nullptr;
	ButtonObject* exit = nullptr;
	ButtonObject* back = nullptr;
	SpriteObject* background = nullptr;
	SpriteObject* tv = nullptr;
	TextObject* nick = nullptr;
	TextObject* nickname = nullptr;
	TextObject* nick_column = nullptr;
	TextObject* score_column = nullptr;
	TextObject* num_column = nullptr;
	TextObject* top = nullptr;
	std::string text;
	bool isActive = false, endAnim = false, ending = false, starting = true;
	bool right = true;
	float timer = 0.f, buffer = 0.5f, dx = 0.f, max_dx = 1920.f, spd = 2.f;
	float play_x, lead_x, exit_x, back_x, alfa = 0.f, nick_x, nickname_x, num_x, nick2_x, scr_x, top_x;


	float endAnimStartBuffor = 0.5f, endAnimEndBuffor = 1.f, endAnimTimer = 0.f;

	void onStart() override;
	void onUpdate(float deltaTime) override;
};

