#pragma once

#include "Script.h"
#include <deque>

class LevelGenerator : public Script
{
public:
	using SelfType = LevelGenerator;

	VARIABLE(bool, generate);
	VARIABLE(bool, playTutorial);
	VARIABLE(int, levelCount);
	VARIABLE(int, levelsRightCount);
	VARIABLE(int, levelsUpCount);
	VARIABLE(int, levelsDownCount);
	VARIABLE(int, minimalHorizontalLevelCount);
	VARIABLE(int, maximalHorizontalLevelCount);
	VARIABLE(int, minimalVerticalLevelCount);
	VARIABLE(int, maximalVerticalLevelCount);
	VARIABLE(float, directionChangeChance);
	VARIABLE(std::string, name);

	bool goingRight = true;
	bool goingUp = false;
	bool goingDown = false;

	int directionLength = 0;
	int roomNumber = 0;
	std::vector<int> usedIndexes;
	std::deque<Node*> currentRooms;

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
		static Variable playTutorialVar = getField_playTutorial();
		static Variable levelCountVar = getField_levelCount();
		static Variable levelsRightCountVar = getField_levelsRightCount();
		static Variable levelsUpCountVar = getField_levelsUpCount();
		static Variable levelsDownCountVar = getField_levelsDownCount();
		static Variable minimalHorizontalLevelCountVar = getField_minimalHorizontalLevelCount();
		static Variable maximalHorizontalLevelCountVar = getField_maximalHorizontalLevelCount();
		static Variable minimalVerticalLevelCountVar = getField_minimalVerticalLevelCount();
		static Variable maximalVerticalLevelCountVar = getField_maximalVerticalLevelCount();
		static Variable directionChangeChanceVar = getField_directionChangeChance();
		static Variable nameVar = getField_name();

		return {
			&generateVar,
			&playTutorialVar,
			&levelCountVar,
			&levelsRightCountVar,
			&levelsUpCountVar,
			&levelsDownCountVar,
			&minimalHorizontalLevelCountVar,
			&maximalHorizontalLevelCountVar,
			&minimalVerticalLevelCountVar,
			&maximalVerticalLevelCountVar,
			&directionChangeChanceVar,
			&nameVar
		};
	}
};

