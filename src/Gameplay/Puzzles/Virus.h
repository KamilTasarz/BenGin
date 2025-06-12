#pragma once

#include "../Script.h"
#include <string>

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
	Node* player = nullptr;

	Virus() = default;
	virtual ~Virus() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onEnd() override;

	//void onCollision(Node* other) override;
	//void onStayCollision(Node* other) override;
	//void onExitCollision(Node* other) override;

	void onCollisionLogic(Node* other) override;
	//void onStayCollisionLogic(Node* other) override;
	//void onExitCollisionLogic(Node* other) override;

	void ApplyEffect(Node* target);
	void VirusEffect(Node* target);

	std::vector<Variable*> getFields() const override {
		//static Variable typeVar = getField_type();
		static Variable blueVar = getField_blue();
		static Variable greenVar = getField_green();
		static Variable blackVar = getField_black();
		return { /*&typeVar, */&blueVar, &greenVar, &blackVar };
	}
};