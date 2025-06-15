#pragma once

#include "Script.h"
#include "../Basic/Node.h"

class TextObject;

class TutorialManager : public Script
{
public:
	using SelfType = TutorialManager;

	VARIABLE(Node*, emitter);

	TextObject* scoreText;
	TextObject* runTimeText;
	TextObject* deathCountText;

	bool showStats = false;
	float timer = 0.f;
	int counter = 0;

	TutorialManager() = default;
	~TutorialManager() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onCollisionLogic(Node* other) override;

	std::vector<Variable*> getFields() const override {
		static Variable emitterVar = getField_emitter();
		return { &emitterVar };
	}
};

