#include "LevelGenerator.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "GameMath.h"
#include "GameManager.h"
#include "../System/PhysicsSystem.h"

REGISTER_SCRIPT(LevelGenerator);

void LevelGenerator::onAttach(Node* owner)
{
	this->owner = owner;
}

void LevelGenerator::onDetach()
{
	owner = nullptr;
}

void LevelGenerator::onStart()
{
	//if (playTutorial) {
		//GameManager::instance().tutorialActive = true;
	//}

	playTutorial = GameManager::instance().tutorialActive;

	if (generate) {
		GenerateLevel();
	}
	else {
		//std::cout << "Level generation is disabled." << std::endl;
	}
}

void LevelGenerator::onUpdate(float deltaTime)
{
}

void LevelGenerator::GenerateLevel()
{
	if (!generate) return;
	
	std::string roomName;
	int levelIndex;
	
	float changeDirection = GameMath::RandomFloat(0, 1);
	
	if (playTutorial) {
		directionLength++;
		roomName = "tutorial_";
		levelIndex = directionLength;
		
		if (directionLength == 6) {
			playTutorial = false;
			directionLength = 0;
			roomNumber = 0;
			usedIndexes.clear();
			currentRooms.clear();
		}
	}
	else if (goingRight) {
		// change direction
		if ((changeDirection < directionChangeChance && directionLength > minimalHorizontalLevelCount) || directionLength >= maximalHorizontalLevelCount) {
			bool chooseUp = !goingDown;

			if (chooseUp) {
				roomName = "room_right_up_";
				goingUp = true;
				goingDown = false;
			}
			else {
				roomName = "room_right_down_";
				goingUp = false;
				goingDown = true;
			}

			levelIndex = 1;
			directionLength = 0;
			usedIndexes.clear();
			goingRight = false;
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
		if ((changeDirection < directionChangeChance && directionLength < minimalVerticalLevelCount) || directionLength >= maximalVerticalLevelCount) {
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
	else if (goingDown) {
		if ((changeDirection < directionChangeChance && directionLength >= minimalVerticalLevelCount) || directionLength >= maximalVerticalLevelCount) {
			roomName = "room_down_right_";
			levelIndex = 1;
			directionLength = 0;

			usedIndexes.clear();
			goingRight = true;
			goingDown = false;
		}
		else {
			roomName = "room_down_";
			if (usedIndexes.size() >= levelsDownCount) {
				usedIndexes.clear();
			}

			do {
				levelIndex = GameMath::RandomInt(1, levelsDownCount);
			} while (std::find(usedIndexes.begin(), usedIndexes.end(), levelIndex) != usedIndexes.end());

			usedIndexes.push_back(levelIndex);
			directionLength++;
		}
	}

	roomNumber++;
	std::string levelName = roomName + std::to_string(levelIndex);

	glm::vec3 pos = owner->getTransform().getLocalPosition();
	PrefabInstance* pref = new PrefabInstance(PrefabRegistry::FindRoomByName(levelName), owner->scene_graph, std::to_string(roomNumber + 20), pos);

	owner->scene_graph->addChild(pref);
	//pref->transform.setLocalPosition(owner->getTransform().getLocalPosition());

	glm::vec3 levelOffset = pref->prefab_root->getChildByTag("Exit")->transform.getLocalPosition();
	owner->transform.setLocalPosition(owner->transform.getLocalPosition() + levelOffset);

	currentRooms.push_back(pref);

	if (currentRooms.size() > 10) {
		Node* toRemove = currentRooms.front();
		//Node* oldest = currentRooms.front();
		//owner->scene_graph->deleteChild(oldest);
		if (toRemove->in_frustrum) return;

		PhysicsSystem::instance().rooms.erase(toRemove);

		currentRooms.pop_front();
		
		owner->scene_graph->deleteChild(toRemove);

	}
}
