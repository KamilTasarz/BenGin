#pragma once

#include "../Script.h"
#include <string>

class VirusRewindable;

class Virus : public Script
{
//private:
//	enum virusType {
//		BLUE,
//		GREEN,
//		BLACK
//	};

public:
	using SelfType = Virus;

	//virusType type = virusType::BLUE;
	//VARIABLE(virusType, type);
	VARIABLE(bool, blue);
	VARIABLE(bool, green);
	VARIABLE(bool, black);

	bool isCollected = false;
	bool modelChanged = false;
	VirusRewindable* rewindable = nullptr;
	std::vector<Node*> particles;
	float particleTimer;

	Virus() = default;
	virtual ~Virus() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onEnd() override;

	void onCollisionLogic(Node* other) override;

	void VirusEffect(Node* target);

	void ShowParticles(std::string prefabName, std::string particleName);

	std::vector<Variable*> getFields() const override {
		//static Variable typeVar = getField_type();
		static Variable blueVar = getField_blue();
		static Variable greenVar = getField_green();
		static Variable blackVar = getField_black();
		return { /*&typeVar, */&blueVar, &greenVar, &blackVar };
	}
};