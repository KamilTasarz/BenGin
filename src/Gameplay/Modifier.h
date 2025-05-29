#pragma once

#include "Script.h"

class Modifier : public Script
{
private:
	enum class ModifierType {
		Slippery,
		COUNT
	};

public:
	ModifierType currentModifier;

	Modifier() = default;
	~Modifier() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onCollisionLogic(Node* other) override;
};
