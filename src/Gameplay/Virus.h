#pragma once

#include "Script.h"

class Virus : public Script
{
private:
	enum virusType {
		BLUE,
		GREEN,
		BLACK
	};

public:
	virusType type = virusType::BLUE; // Default type

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
};