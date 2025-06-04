#pragma once

#include "../Script.h"
#include <glm/gtc/quaternion.hpp>

class MirrorRotator : public Script
{
public:
	using SelfType = MirrorRotator;

	VARIABLE(Node*, mirrorNode);
	VARIABLE(float, movingRange);
	VARIABLE(float, rotationAngle);

	glm::vec3 startPosition;
	glm::quat startRotation;

	MirrorRotator() = default;
	~MirrorRotator() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;

	std::vector<Variable*> getFields() const override {
		static Variable mirrorNodeVar = getField_mirrorNode();
		static Variable movingRangeVar = getField_movingRange();
		static Variable rotationAngleVar = getField_rotationAngle();
		return { &mirrorNodeVar, &movingRangeVar, &rotationAngleVar };
	}
};

