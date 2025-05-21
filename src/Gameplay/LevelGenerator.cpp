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
	for (int i = 0; i < levelCount; ++i) {
		int levelIndex = GameMath::RandomInt(3, 4);
		std::string levelName = "room_" + std::to_string(levelIndex);

		PrefabInstance* pref = new PrefabInstance(PrefabRegistry::FindByName(levelName), owner->scene_graph, std::to_string(i + 20));
		//Node* player = pref->prefab_root->getChildByTag("Player");
		//player->transform.setLocalPosition(owner->getTransform().getLocalPosition());
		owner->scene_graph->addChild(pref);
		pref->transform.setLocalPosition(owner->getTransform().getLocalPosition());

		glm::vec3 levelOffset = pref->prefab_root->getChildByTag("Exit")->transform.getLocalPosition();
		owner->transform.setLocalPosition(owner->transform.getLocalPosition() + levelOffset);
		//owner->forceUpdateSelfAndChild();
	}
}
