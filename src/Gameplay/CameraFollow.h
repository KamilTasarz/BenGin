#pragma once

#include "Script.h"

class CameraFollow : public Script
{
public:
	using SelfType = CameraFollow;

	VARIABLE(Node*, player);
	VARIABLE(float, offsetY);
	VARIABLE(float, offsetZ);
	VARIABLE(float, smoothing);

	CameraFollow() = default;
	virtual ~CameraFollow() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onEnd() override;

	std::vector<Variable*> getFields() const override {
		static Variable playerVar = getField_player();
		static Variable offsetYVar = getField_offsetY();
		static Variable offsetZVar = getField_offsetZ();
		static Variable speedVar = getField_smoothing();

		return { &playerVar, &offsetYVar, &offsetZVar, &speedVar };
	}
};

