#include "PlayerSpawner.h"
#include "../Basic/Node.h"
#include "../System/PrefabRegistry.h"
#include "RegisterScript.h"

REGISTER_SCRIPT(PlayerSpawner);

void PlayerSpawner::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "PlayerSpawner::onAttach::" << owner->name << std::endl;
}

void PlayerSpawner::onDetach()
{
	std::cout << "PlayerSpawner::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void PlayerSpawner::onStart()
{
	i = 12;
	spawnPlayer();
}

void PlayerSpawner::onUpdate(float deltaTime)
{
}

void PlayerSpawner::onEnd()
{
}

void PlayerSpawner::spawnPlayer()
{
	i++;
	std::cout << "Spawning player" << std::endl;

	PrefabInstance *pref = new PrefabInstance(PrefabRegistry::FindPuzzleByName("Player"), owner->scene_graph, "_" + std::to_string(i), owner->getTransform().getLocalPosition());

	Node* player = pref->prefab_root->getChildByTag("Player");
	player->transform.setLocalPosition(owner->getTransform().getLocalPosition());
 	owner->scene_graph->addChild(player);
}