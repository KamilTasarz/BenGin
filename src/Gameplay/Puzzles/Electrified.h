#pragma once

#include "../Script.h"
#include "../Particles.h"

class Electrified : public Script
{
public:
	using SelfType = Electrified;
	VARIABLE(bool, isActive);

	int sfxId = -1;
	Particles* particleEmitter = nullptr;

	Electrified() = default;
	~Electrified() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime);
	//void onUpdate(float deltaTime) override;
	void onCollisionLogic(Node* other) override;
	std::vector<Variable*> getFields() const override {
		static Variable isActiveVar = getField_isActive();
		return { &isActiveVar };
	}
};

