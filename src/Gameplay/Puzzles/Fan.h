#pragma once

#include "../Script.h"

class Fan : public Script
{
public:
	using SelfType = Fan;

	VARIABLE(float, verticalPower);
	VARIABLE(float, horizontalPower);
	VARIABLE(bool, isActive);

	float wavyPower;
	int sfxId = -1;
	Node* fan = nullptr;
	float fanSpeed = 0.0f;
	float targetFanSpeed = 0.0f;

	Fan() = default;
	virtual ~Fan() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onStayCollisionLogic(Node* other) override;
	void onCollisionLogic(Node* other);

	std::vector<Variable*> getFields() const override {
		static Variable verticalPowerVar = getField_verticalPower();
		static Variable horizontalPowerVar = getField_horizontalPower();
		static Variable isActiveVar = getField_isActive();
		return { &verticalPowerVar, &horizontalPowerVar, &isActiveVar };
	}
};

