#pragma once

#include "Script.h"

class PowerActivator : public Script
{
public:
	using SelfType = PowerActivator;
	
	VARIABLE(Node*, object);
	VARIABLE(bool, activate)
	VARIABLE(bool, isActivated);

	PowerActivator() = default;
	~PowerActivator() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onStayCollisionLogic(Node* other) override;
	void ChangeState(bool state);
	std::vector<Variable*> getFields() const override {
		static Variable objectVar = getField_object();
		static Variable activateVar = getField_activate();
		static Variable isActivatedVar = getField_isActivated();
		return { &objectVar, &activateVar, &isActivatedVar };
	}
};

