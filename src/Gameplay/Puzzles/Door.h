#pragma once

#include "../Script.h"

class Door : public Script
{
public:
	using SelfType = Door;

	VARIABLE(bool, isOpen);
	VARIABLE(bool, openToSide);
	glm::vec3 targetPos;
	glm::vec3 startPos;

	Door() = default;
	~Door() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void ChangeState(bool state);

	std::vector<Variable*> getFields() const override {
		static Variable isOpenVar = getField_isOpen();
		static Variable openToSideVar = getField_openToSide();
		return { &isOpenVar, &openToSideVar };
	}
};

