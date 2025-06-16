#pragma once

#include "Script.h"

class PlayerSpawner : public Script
{
public:
	using SelfType = PlayerSpawner;

	int i = 12;
	bool spawnInNextFrame = false;

	PlayerSpawner() = default;
	virtual ~PlayerSpawner() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void onEnd() override;
	void spawnPlayer();

	/*std::vector<Variable*> getFields() const override {
		static Variable spawnXVar = getField_spawnX();
		static Variable spawnYVar = getField_spawnY();
		static Variable spawnZVar = getField_spawnZ();
		return { &spawnXVar, &spawnYVar, &spawnZVar };
	}*/
};

