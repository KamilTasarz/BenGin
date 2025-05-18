#include "PlayerSpawner.h"
#include "../Basic/Node.h"
#include "../System/Editor.h"
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
}

void PlayerSpawner::onUpdate(float deltaTime)
{
}

void PlayerSpawner::onEnd()
{
}

void PlayerSpawner::spawnPlayer()
{
	//Prefab* player = new Prefab("Player");
	//owner->scene_graph->addChild(new PrefabInstance(player, owner->scene_graph));
}
