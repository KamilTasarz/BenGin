#pragma once

#include "../Script.h"

class LaserEmitter : public Script
{
public:
	using SelfType = LaserEmitter;

	VARIABLE(bool, isOpen);
	glm::vec3 targetPos;
	glm::vec3 startPos;

	LaserEmitter() = default;
	~LaserEmitter() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void ChangeState(bool state);

	std::vector<Variable*> getFields() const override {
		static Variable isOpenVar = getField_isOpen();
		return { &isOpenVar };
	}
};

