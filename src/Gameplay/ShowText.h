#pragma once

#include "Script.h"

class TextObject;

class ShowText : public Script
{
public:
	using SelfType = ShowText;

	VARIABLE(std::string, text);
	VARIABLE(std::string, textSecond);
	VARIABLE(float, speed);

	//std::string text = "Escape from the gas!";

	float timer = -1.f;
	TextObject* textObject;
	TextObject* textObjectSecond;
	bool isWriting = false;
	bool isDeleting = false;
	bool entered = false;
	bool writingFirstDone = false;
	vector<char> textChars;

	ShowText() = default;
	~ShowText() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;

	void onCollisionLogic(Node* other) override;
	void onExitCollisionLogic(Node* other) override;

	std::vector<Variable*> getFields() const override {
		static Variable textVar = getField_text();
		static Variable textSecondVar = getField_textSecond();
		static Variable speedVar = getField_speed();
		return { &textVar, &textSecondVar, &speedVar };
	}
};

