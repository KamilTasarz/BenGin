#include "GameManager.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "PlayerSpawner.h"
#include "LevelGenerator.h"
#include "../System/GuiManager.h"

REGISTER_SCRIPT(GameManager);

GameManager* GameManager::instance = nullptr;

void GameManager::onAttach(Node* owner)
{
    this->owner = owner;
    GameManager::instance = this;
}

void GameManager::onDetach()
{
    owner = nullptr;
}

void GameManager::onStart()
{

    playerSpawner = owner->scene_graph->root->getChildByTag("PlayerSpawner");
	playerSpawner->getComponent<PlayerSpawner>()->spawnPlayer();

    levelGenerator = owner->scene_graph->root->getChildByTag("LevelGenerator");
    emitter = dynamic_cast<InstanceManager*>(owner->scene_graph->root->getChildByTag("Emitter"));

    scoreText = GuiManager::Instance().findText(0);
	runTimeText = GuiManager::Instance().findText(1);
	deathCountText = GuiManager::Instance().findText(2);
	fpsText = GuiManager::Instance().findText(5);

	runTime = 0.f;
	score = 0.f;
	deathCount = 0;
}

void GameManager::onUpdate(float deltaTime)
{
	runTime += deltaTime;
	score += deltaTime * 10.f / gasSpreadingSpeed;
    
	int minutes = static_cast<int>(runTime) / 60;
	int seconds = static_cast<int>(runTime) % 60;

	std::string minutesText = minutes < 10 ? "0" + std::to_string(minutes) : std::to_string(minutes);
	std::string secondsText = seconds < 10 ? "0" + std::to_string(seconds) : std::to_string(seconds);
	runTimeText->value = minutesText + ":" + secondsText;

    std::string scoreWithZeros = std::format("{:05}", static_cast<int>(score));
	scoreText->value = "SCORE:" + scoreWithZeros;

	deathCountText->value = "DEATHS:" + std::to_string(deathCount);

    timeSinceLastUpdate += deltaTime;
    fpsAccumulator += 1.f / deltaTime; // fps dla tej jednej klatki
    frameCount++;

    // co 0.5 sekundy aktualizujemy wynik
    if (timeSinceLastUpdate >= updateInterval) {
        float avgFps = fpsAccumulator / frameCount;
        fpsText->value = "FPS: " + std::to_string(static_cast<int>(avgFps));

        // resetujemy akumulator
        timeSinceLastUpdate = 0.f;
        fpsAccumulator = 0.f;
        frameCount = 0;
    }

    CalculateGasSpreadingSpeed(deltaTime);
    HandleLevelGeneration();
}

void GameManager::CalculateGasSpreadingSpeed(float deltaTime) {
    int index = emitter->tail - 1;

    glm::vec3 gasPos = emitter->particles[index].position;
    glm::vec3 spawnerPos = playerSpawner->transform.getGlobalPosition();

    float distanceFromGasToSpawner = glm::distance(gasPos, spawnerPos);

    gasSpreadingSpeed = 10.f / distanceFromGasToSpawner;
    gasSpreadingSpeed = glm::clamp(gasSpreadingSpeed, 0.3f, 3.f);

    std::cout << gasSpreadingSpeed << std::endl;
}

void GameManager::HandleLevelGeneration() {
    glm::vec3 playerPos = currentPlayer->transform.getGlobalPosition();
    glm::vec3 generatorPos = levelGenerator->transform.getGlobalPosition();

    float distanceToLevelGen = glm::distance(playerPos, generatorPos);

    if (distanceToLevelGen < 30.f) {
        levelGenerator->getComponent<LevelGenerator>()->GenerateLevel();
    }
}