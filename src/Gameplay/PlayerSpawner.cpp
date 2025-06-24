#include "PlayerSpawner.h"
#include "../Basic/Node.h"
#include "../System/PrefabRegistry.h"
#include "RegisterScript.h"
#include "GameManager.h"
#include "PlayerController.h"
#include "../Basic/Animator.h"

REGISTER_SCRIPT(PlayerSpawner);

void PlayerSpawner::onAttach(Node* owner)
{
	this->owner = owner;
	//std::cout << "PlayerSpawner::onAttach::" << owner->name << std::endl;
}

void PlayerSpawner::onDetach()
{
	//std::cout << "PlayerSpawner::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void PlayerSpawner::onStart()
{
	i = 12;
	//spawnPlayer();
}

void PlayerSpawner::onUpdate(float deltaTime)
{
	if (GameManager::instance().isRewinding) return;
	
	if (spawnInNextFrame) {
		spawnPlayer();
		spawnInNextFrame = false;
	}
	
	if (GameManager::instance().currentPlayer == nullptr) {
		spawnInNextFrame = true;
	}
	else {
		PlayerController* player = GameManager::instance().currentPlayer->getComponent<PlayerController>();
		if (player != nullptr && player->isDead) {
			spawnInNextFrame = true;
		}
	}
}

void PlayerSpawner::onEnd()
{
}

void PlayerSpawner::spawnPlayer()
{
	i++;
	//std::cout << "Spawning player" << std::endl;

	PrefabInstance *pref = new PrefabInstance(PrefabRegistry::FindPuzzleByName("Player"), owner->scene_graph, "_" + std::to_string(i), owner->getTransform().getLocalPosition());

	Node* player = pref->prefab_root->getChildByTag("Player");
	player->transform.setLocalPosition(owner->getTransform().getLocalPosition());
 	owner->scene_graph->addChild(player);
	player->animator->current_animation = player->pModel->getAnimationByName("Idle");

	//currentPlayer = player;
	GameManager::instance().currentPlayer = player;
	GameManager::instance().players.push_back(player);
}