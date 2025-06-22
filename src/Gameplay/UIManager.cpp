#include "UIManager.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "../System/GuiManager.h"
#include "GameManagerRewindable.h"
#include "GameManager.h"

REGISTER_SCRIPT(UIManager);

void UIManager::onAttach(Node* owner)
{
	this->owner = owner;
}

void UIManager::onDetach()
{
	owner = nullptr;
}

void UIManager::onStart()
{
	scoreText = GuiManager::Instance().findText(0);
	runTimeText = GuiManager::Instance().findText(1);
	deathCountText = GuiManager::Instance().findText(2);
	stateText = GuiManager::Instance().findText(3);
	fpsText = GuiManager::Instance().findText(5);
	timeline = GuiManager::Instance().findText(11);
	timelineTop = GuiManager::Instance().findText(12);
	timelineDown = GuiManager::Instance().findText(13);
	playSprite = GuiManager::Instance().findSprite(15);
	rewindSprite = GuiManager::Instance().findSprite(16);

    rewindable = owner->getComponent<GameManagerRewindable>();
    if (rewindable == nullptr) {
        owner->addComponent(std::make_unique<GameManagerRewindable>());
        rewindable = owner->getComponent<GameManagerRewindable>();
    }
}

void UIManager::onUpdate(float deltaTime)
{
    isRewinding = rewindable->isRewinding;
    historyEmpty = rewindable->history.empty();
    GameManager::instance().isRewinding = isRewinding;
    GameManager::instance().historyEmpty = historyEmpty;
    
    if (isRewinding) {
        stateText->value = "REWIND";
        rewindSprite->visible = true;
        playSprite->visible = false;

        HandleRewindTimeline();
        timeline->visible = true;
        timelineTop->visible = true;
        timelineDown->visible = true;
    }
    else {
        stateText->value = "PLAY";
        rewindSprite->visible = false;
        playSprite->visible = true;

        startHistorySize = rewindable->history.size();
        timeline->visible = false;
        timelineTop->visible = false;
        timelineDown->visible = false;
    }

    if (!GameManager::instance().tutorialActive) {
        runTime = GameManager::instance().runTime;
        score = GameManager::instance().score;
        deathCount = GameManager::instance().deathCount;

        int minutes = static_cast<int>(runTime) / 60;
        int seconds = static_cast<int>(runTime) % 60;

        std::string minutesText = minutes < 10 ? "0" + std::to_string(minutes) : std::to_string(minutes);
        std::string secondsText = seconds < 10 ? "0" + std::to_string(seconds) : std::to_string(seconds);
        runTimeText->value = minutesText + ":" + secondsText;

        std::string scoreWithZeros = std::format("{:05}", static_cast<int>(score));
        scoreText->value = "SCORE:" + scoreWithZeros;

        deathCountText->value = "DEATHS:" + std::to_string(deathCount);
    }
    else if (GameManager::instance().tutorialActive && scoreText->visible) {
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
}

void UIManager::HandleRewindTimeline()
{
    int timelineSize = 29;
    int historySize = rewindable->history.size();
    int mappedHistorySize = static_cast<int>((historySize / (float)startHistorySize) * timelineSize);

    std::string speed = historySize > 0 ? std::to_string(rewindable->rewindSpeed) + +"x " : "-:- ";
    std::string timelineText = speed + std::string(mappedHistorySize, '|') + std::string(timelineSize - mappedHistorySize, '-');

    timeline->value = timelineText;
}
