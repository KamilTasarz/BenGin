#pragma once

#include "Script.h"

class CameraProp : public Script
{
public:
	CameraProp() = default;
	virtual ~CameraProp() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
};