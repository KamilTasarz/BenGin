#pragma once

#include "../Script.h"

class PointLight;

class Door : public Script
{
public:
	using SelfType = Door;

	VARIABLE(bool, isOpen);
	VARIABLE(bool, openToSide);
	VARIABLE(bool, activateAlarm);
	PointLight* light = nullptr;

	glm::vec3 targetPos;
	glm::vec3 startPos;
	glm::vec3 targetPos2;
	glm::vec3 startPos2;
	bool overrideState = false;
	int sfxId = -1;

	Node* door1 = nullptr;
	Node* door2 = nullptr;

	Door() = default;
	~Door() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void ChangeState(bool state);
	void onCollisionLogic(Node* other) override;

	std::vector<Variable*> getFields() const override {
		static Variable isOpenVar = getField_isOpen();
		static Variable openToSideVar = getField_openToSide();
		static Variable activateAlarmVar = getField_activateAlarm();
		return { &isOpenVar, &openToSideVar, &activateAlarmVar };
	}
};

