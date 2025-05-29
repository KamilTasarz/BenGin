#include "GameManager.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"

REGISTER_SCRIPT(GameManager);

GameManager* GameManager::instance = nullptr;

void GameManager::onAttach(Node* owner)
{
    this->owner = owner;
}

void GameManager::onDetach()
{
    owner = nullptr;
}

void GameManager::onStart()
{
    GameManager::instance = this;
}

void GameManager::onUpdate(float deltaTime)
{
}