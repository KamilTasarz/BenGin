#include "LevelGenerator.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "GameMath.h"

REGISTER_SCRIPT(LevelGenerator);

void LevelGenerator::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "LevelGenerator::onAttach::" << owner->name << std::endl;
}

void LevelGenerator::onDetach()
{
	std::cout << "LevelGenerator::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void LevelGenerator::onStart()
{
	std::cout << "LevelGenerator::onStart::" << owner->name << std::endl;

	if (generate) {
		GenerateLevel();
	}
	else {
		std::cout << "Level generation is disabled." << std::endl;
	}
}

void LevelGenerator::onUpdate(float deltaTime)
{
}

void LevelGenerator::GenerateLevel()
{
	std::string roomName;
	int levelIndex;
	
	//for (int i = 0; i < levelCount; ++i) {
		float changeDirection = GameMath::RandomFloat(0, 1);
		
		if (goingRight) {
			// change direction
			if ((changeDirection < directionChangeChance && directionLength > minimalHorizontalLevelCount) || directionLength > maximalHorizontalLevelCount) {
				roomName = "room_right_up_";
				levelIndex = 1;
				directionLength = 0;

				usedIndexes.clear();
				goingRight = false;
				goingUp = true;
			}
			// go right
			else {
				roomName = "room_";
				levelIndex = GameMath::RandomInt(1, levelsRightCount) + 2;

				if (usedIndexes.size() >= levelsRightCount) {
					usedIndexes.clear();
				}

				do {
					levelIndex = GameMath::RandomInt(1, levelsRightCount) + 2;
				} while (std::find(usedIndexes.begin(), usedIndexes.end(), levelIndex) != usedIndexes.end());

				usedIndexes.push_back(levelIndex);
				directionLength++;
			}
		}
		else if (goingUp) {
			// change direction
			if ((changeDirection < directionChangeChance && directionLength < minimalVerticalLevelCount) || directionLength > maximalVerticalLevelCount) {
				roomName = "room_up_right_";
				levelIndex = 1;
				directionLength = 0;

				usedIndexes.clear();
				goingRight = true;
				goingUp = false;
			}
			// go up
			else {
				roomName = "room_up_";
				levelIndex = GameMath::RandomInt(1, levelsUpCount);

				if (usedIndexes.size() >= levelsUpCount) {
					usedIndexes.clear();
				}

				do {
					levelIndex = GameMath::RandomInt(1, levelsUpCount);
				} while (std::find(usedIndexes.begin(), usedIndexes.end(), levelIndex) != usedIndexes.end());

				usedIndexes.push_back(levelIndex);

				directionLength++;
			}
		}

		roomNumber++;
		std::string levelName = roomName + std::to_string(levelIndex);
		PrefabInstance* pref = new PrefabInstance(PrefabRegistry::FindRoomByName(levelName), owner->scene_graph, std::to_string(roomNumber + 20));

		owner->scene_graph->addChild(pref);
		pref->transform.setLocalPosition(owner->getTransform().getLocalPosition());

		glm::vec3 levelOffset = pref->prefab_root->getChildByTag("Exit")->transform.getLocalPosition();
		owner->transform.setLocalPosition(owner->transform.getLocalPosition() + levelOffset);

		currentRooms.push_back(pref);

		if (currentRooms.size() > 3) {
			//Node* oldest = currentRooms.front();
			//owner->scene_graph->deleteChild(oldest);

			currentRooms.pop_front();
		}
	//}
}
