#pragma once

#include "../Script.h"
#include "../NPC.h"

class TVManager : public Script
{
public:
	using SelfType = TVManager;

	VARIABLE(Node*, tv);
	VARIABLE(Node*, cheese);

	bool isFleeing = false;
	NPC* tvAI;

	glm::vec3 tvStartPos;
	glm::vec3 cheeseStartPos;

	glm::vec3 targetVelocity;
	glm::vec3 currentVelocity = glm::vec3(0.f, 0.f, 3.f);

	TVManager() = default;
	virtual ~TVManager() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;

	std::vector<Variable*> getFields() const override {
		static Variable tvVar = getField_tv();
		static Variable cheeseVar = getField_cheese();
		return { &tvVar, &cheeseVar };
	}
};

