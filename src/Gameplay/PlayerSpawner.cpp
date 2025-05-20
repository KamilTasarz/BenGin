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
}

void PlayerSpawner::onUpdate(float deltaTime)
{
	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_P) == GLFW_PRESS) {

		spawnPlayer();
	}
}

void PlayerSpawner::onEnd()
{
}

void PlayerSpawner::spawnPlayer()
{
	std::cout << "Spawning player" << std::endl;
	PrefabInstance *pref = new PrefabInstance(PrefabRegistry::FindByName("Player"), owner->scene_graph, owner->getTransform().getLocalPosition());
	Node* player = pref->prefab_root->getChildByTag("Player");
	player->transform.setLocalPosition(owner->getTransform().getLocalPosition());
 	owner->scene_graph->addChild(player);
}