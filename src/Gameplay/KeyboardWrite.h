#pragma once
#include "Script.h"

class TextObject;

class KeyboardWrite : public Script
{
public:
	using SelfType = KeyboardWrite;
	
	TextObject* textObject = nullptr;
	std::string text;
	bool isActive = false;
	bool isPressed = false;
	bool isPressed_back = false;
	bool lastSign = false;
	float timer = 0.f, buffer = 0.5f;


	std::unordered_set<int> pressedKeys;

	void onStart() override;
	void onUpdate(float deltaTime) override;
};

