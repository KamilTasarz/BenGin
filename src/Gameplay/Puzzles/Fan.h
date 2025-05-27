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

	Fan() = default;
	virtual ~Fan() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onStayCollisionLogic(Node* other) override;
	void onExitCollisionLogic(Node* other) override;
	//void onEnd() override;

	std::vector<Variable*> getFields() const override {
		static Variable verticalPowerVar = getField_verticalPower();
		static Variable horizontalPowerVar = getField_horizontalPower();
		static Variable isActiveVar = getField_isActive();
		return { &verticalPowerVar, &horizontalPowerVar, &isActiveVar };
	}
};

