#pragma once

#include "Script.h"

class GasParticle : public Script
{
public:
	float growTime = 1.f;
	float timer;
	glm::vec3 scale;

	GasParticle() = default;
	~GasParticle() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;

	void onStayCollisionLogic(Node* other) override;
};

