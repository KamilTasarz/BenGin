#pragma once

#include "Script.h"

class LevelGenerator : public Script
{
public:
	using SelfType = LevelGenerator;

	VARIABLE(bool, generate);
	VARIABLE(int, levelCount);
	VARIABLE(int, levelsRightCount);
	VARIABLE(int, levelsUpCount);
	VARIABLE(int, levelsDownCount);
	VARIABLE(int, minimalHorizontalLevelCount);
	VARIABLE(int, maximalHorizontalLevelCount);
	VARIABLE(int, minimalVerticalLevelCount);
	VARIABLE(int, maximalVerticalLevelCount);
	VARIABLE(float, directionChangeChance);

	bool goingRight = true;
	bool goingUp = false;
	bool goingDown = false;

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
		static Variable levelsRightCountVar = getField_levelsRightCount();
		static Variable levelsUpCountVar = getField_levelsUpCount();
		static Variable levelsDownCountVar = getField_levelsDownCount();
		static Variable minimalHorizontalLevelCountVar = getField_minimalHorizontalLevelCount();
		static Variable maximalHorizontalLevelCountVar = getField_maximalHorizontalLevelCount();
		static Variable minimalVerticalLevelCountVar = getField_minimalVerticalLevelCount();
		static Variable maximalVerticalLevelCountVar = getField_maximalVerticalLevelCount();
		static Variable directionChangeChanceVar = getField_directionChangeChance();

		return {
			&generateVar,
			&levelCountVar,
			&levelsRightCountVar,
			&levelsUpCountVar,
			&levelsDownCountVar,
			&minimalHorizontalLevelCountVar,
			&maximalHorizontalLevelCountVar,
			&minimalVerticalLevelCountVar,
			&maximalVerticalLevelCountVar,
			&directionChangeChanceVar
		};
	}
};

