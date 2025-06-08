#pragma once

#include "../Script.h"
#include "../../System/Rigidbody.h"

class Box : public Script
{
public:
	Rigidbody* rb = nullptr;
	int sfxId = -1;

	Box() = default;
	~Box() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
};

