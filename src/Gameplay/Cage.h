#pragma once

#include "Script.h"

class Cage final : public Script
{
public:
	using SelfType = Cage;

	VARIABLE(float, openingSpeed);

	bool isOpen = false;
	float timer = 3.f;

	Cage() = default;
	~Cage() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;

	std::vector<Variable*> getFields() const override {
		static Variable openingSpeedVar = getField_openingSpeed();
		return { &openingSpeedVar };
	}
};