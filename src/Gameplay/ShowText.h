#pragma once

#include "Script.h"

class TextObject;

class ShowText : public Script
{
public:
	using SelfType = ShowText;

	//VARIABLE(std::string, text);
	VARIABLE(float, speed);

	std::string text = "Escape from the gas!";

	TextObject* textObject;
	bool isWriting = false;
	bool isDeleting = false;
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
		//static Variable textVar = getField_text();
		static Variable speedVar = getField_speed();
		return { /*&textVar,*/ &speedVar };
	}
};

