#pragma once

#include "Script.h"

class Electrified : public Script
{
public:
	/*using SelfType = Electrified;
	VARIABLE(float, damage);
	VARIABLE(float, duration);*/

	Electrified() = default;
	~Electrified() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	//void onStart() override;
	//void onUpdate(float deltaTime) override;
	void onCollisionLogic(Node* other) override;
	/*std::vector<Variable*> getFields() const override {
		static Variable damageVar = getField_damage();
		static Variable durationVar = getField_duration();
		return { &damageVar, &durationVar };
	}*/
};

