#include "GameManager.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "PlayerSpawner.h"
#include "LevelGenerator.h"
#include "../System/GuiManager.h"
#include "GameManagerRewindable.h"
#include "PlayerRewindable.h"
#include "RewindManager.h"

REGISTER_SCRIPT(GameManager);

GameManager* GameManager::instance = nullptr;

void GameManager::onAttach(Node* owner)
{
	GameManager::instance = this;
    this->owner = owner;
}

void GameManager::onDetach()
{
    owner = nullptr;
}

void GameManager::onStart()
{

    playerSpawner = owner->scene_graph->root->getChildByTag("PlayerSpawner");
	//playerSpawner->getComponent<PlayerSpawner>()->spawnPlayer();

    levelGenerator = owner->scene_graph->root->getChildByTag("LevelGenerator")->getComponent<LevelGenerator>();
    emitter = dynamic_cast<InstanceManager*>(owner->scene_graph->root->getChildByTag("Emitter"));

    scoreText = GuiManager::Instance().findText(0);
	runTimeText = GuiManager::Instance().findText(1);
	deathCountText = GuiManager::Instance().findText(2);
	stateText = GuiManager::Instance().findText(3);
	fpsText = GuiManager::Instance().findText(5);
	playSprite = GuiManager::Instance().findSprite(15);
	rewindSprite = GuiManager::Instance().findSprite(16);

	runTime = 0.f;
	score = 0.f;
	deathCount = 0;

	rewindable = owner->getComponent<GameManagerRewindable>();
	if (rewindable == nullptr) {
		owner->addComponent(std::make_unique<GameManagerRewindable>());
		rewindable = owner->getComponent<GameManagerRewindable>();
	}
}

void GameManager::onUpdate(float deltaTime)
{
	isRewinding = rewindable->isRewinding;
    
    if (isRewinding) {
        stateText->value = "REWIND";
		rewindSprite->visible = true;
		playSprite->visible = false;
	}
    else {
        stateText->value = "PLAY";
        rewindSprite->visible = false;
        playSprite->visible = true;
    }

    if (!tutorialActive) {
        if (!isRewinding) {
            runTime += deltaTime;
            score += deltaTime * 10.f / gasSpreadingSpeed;
        }

        int minutes = static_cast<int>(runTime) / 60;
        int seconds = static_cast<int>(runTime) % 60;

        std::string minutesText = minutes < 10 ? "0" + std::to_string(minutes) : std::to_string(minutes);
        std::string secondsText = seconds < 10 ? "0" + std::to_string(seconds) : std::to_string(seconds);
        runTimeText->value = minutesText + ":" + secondsText;

        std::string scoreWithZeros = std::format("{:05}", static_cast<int>(score));
        scoreText->value = "SCORE:" + scoreWithZeros;

        deathCountText->value = "DEATHS:" + std::to_string(deathCount);
    }
    else if (tutorialActive && scoreText->visible) {
        scoreText->visible = false;
        runTimeText->visible = false;
        deathCountText->visible = false;
    }

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

    if (players.size() > 3) {
        RemovePlayer();
    }

    CalculateGasSpreadingSpeed(deltaTime);
    HandleLevelGeneration();
}

void GameManager::RemovePlayer()
{
	if (players.empty()) return;

	Node* playerToRemove = players.front();
	owner->scene_graph->deleteChild(playerToRemove);
    players.pop_front();
}

void GameManager::RemoveCurrentPlayer()
{
    if (players.size() <= 1) return;
    Node* playerToRemove = players.back();

    auto* player = playerToRemove->getComponent<PlayerRewindable>();
    RewindManager::Instance().unregisterRewindable(player);

    owner->scene_graph->deleteChild(playerToRemove);
    players.pop_back();

    if (!players.empty()) currentPlayer = players.back();
}

void GameManager::CalculateGasSpreadingSpeed(float deltaTime) {
    int index = emitter->tail - 1;

    glm::vec3 spawnerPos = playerSpawner->transform.getGlobalPosition();

    glm::vec3 gasPos = emitter->particles[index].position;
    float distanceFromGasToSpawner = glm::distance(gasPos, spawnerPos);

    float minDistance = 100.f;
	int size = clamp(index, 0, 15);

	for (int i = 0; i < size; i++) {
		glm::vec3 particlePos = emitter->particles[index - i].position;
		float distance = glm::distance(particlePos, spawnerPos);
		if (i == 0 || distance < minDistance) {
			minDistance = distance;
		}
	}

    gasSpreadingSpeed = 10.f / minDistance;

	if (tutorialActive) {
        gasSpreadingSpeed = glm::clamp(gasSpreadingSpeed, 0.6f, 200.f);
	}
    else {
        gasSpreadingSpeed = glm::clamp(gasSpreadingSpeed, 0.3f, 3.f);
        float modifier = 1.f + runTime / 300.f;
		gasSpreadingSpeed /= modifier;
    }

    std::cout << gasSpreadingSpeed << std::endl;
}

void GameManager::HandleLevelGeneration() {
    if (!currentPlayer) return;
    glm::vec3 playerPos = currentPlayer->transform.getGlobalPosition();
    glm::vec3 generatorPos = levelGenerator->getOwner()->transform.getGlobalPosition();

    float distanceToLevelGen = glm::distance(playerPos, generatorPos);

    if (distanceToLevelGen < 40.f) {
        levelGenerator->GenerateLevel();
    }
}