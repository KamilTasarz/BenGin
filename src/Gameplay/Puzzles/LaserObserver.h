#pragma once

#include "../Script.h"

class LaserObserver : public Script
{
public:
	using SelfType = LaserObserver;

	VARIABLE(Node*, object);
	VARIABLE(bool, activate)
	VARIABLE(bool, isActivated);

	float deactivationTimer;
	bool activated = false;

	LaserObserver() = default;
	~LaserObserver() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void ChangeState(bool state);
	void Activate();

	std::vector<Variable*> getFields() const override {
		static Variable objectVar = getField_object();
		static Variable activateVar = getField_activate();
		static Variable isActivatedVar = getField_isActivated();
		return { &objectVar, &activateVar, &isActivatedVar };
	}
};

