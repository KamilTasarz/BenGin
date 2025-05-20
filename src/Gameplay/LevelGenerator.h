#pragma once

#include "Script.h"

class LevelGenerator : public Script
{
public:
	using SelfType = LevelGenerator;

	VARIABLE(bool, generate);
	VARIABLE(int, levelCount);
	


	using SelfType = LevelGenerator;
	LevelGenerator() = default;
	~LevelGenerator() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void GenerateLevel();

	std::vector<Variable*> getFields() const override {
		static Variable generateVar = getField_generate();
		static Variable levelCountVar = getField_levelCount();
		return { &generateVar, &levelCountVar };
	}
};

