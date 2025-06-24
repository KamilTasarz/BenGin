#include "MusicManager.h"
#include "../AudioEngine.h"
#include "../System/ServiceLocator.h"
#include "GameManager.h"
#include "PlayerController.h"

MusicManager& MusicManager::instance()
{
    static MusicManager instance;
    return instance;
}

void MusicManager::Init()
{
    menuVolume = volume;

	auto* audio = ServiceLocator::getAudioEngine();
	menuId = audio->PlayMusic(audio->musicMenu, GameManager::instance().sfxVolume * menuVolume);

	menuActive = true;
}

void MusicManager::Update(float deltaTime)
{
    auto* audio = ServiceLocator::getAudioEngine();
    Node* player = GameManager::instance().currentPlayer;
    if (!player) return;

    if (menuActive) {
        return;
    }
    else if (transitionActive) {
        if (audio->IsPlaying(transitionId)) {

            if (audio->IsPlaying(menuId)) {
                menuVolume = glm::mix(menuVolume, 0.f, deltaTime * 2.f);
                audio->SetChannelvolume(menuId, menuVolume);
            }
            else {
                audio->stopSound(menuId);
            }

            return;
        }
        else {
            StartGameMusic();
            transitionActive = false;
        }
    }

    /*if (GameManager::instance().isRewinding && !rewindActive) {
        PlayRewindSound();
    }
    else if (!GameManager::instance().isRewinding && rewindActive) {
        StopRewindSound();
    }*/

    if (rewindActive) {
        return;
    }

    float fadeSpeed = 1.f;
    float targetVolume = GameManager::instance().sfxVolume * volume;

    float distance = GameManager::instance().minPlayerToParticleDistance;
    bool isInGas = player->getComponent<PlayerController>()->isInGas;

    int targetStage = getTargetStage(distance, isInGas);

    std::cout << "Distance to gas: " << distance << ", targetStage: " << targetStage << std::endl;

    // plynna zmiana

    for (int i = 0; i < 4; ++i) {
        float desiredVolume = (i + 1 == targetStage) ? targetVolume : 0.f;
        stageVolumes[i] = glm::mix(stageVolumes[i], desiredVolume, deltaTime / fadeSpeed);
        audio->SetChannelvolume(stageChannels[i], stageVolumes[i] * 0.3f);
    }

    // zmiana co takt

    /*if (timer < 0.f) {
        timer = beatTime + timer;

        for (int i = 0; i < 4; ++i) {
            float desiredVolume = (i + 1 == targetStage) ? targetVolume : 0.f;
            stageVolumes[i] = desiredVolume;
            audio->SetChannelvolume(stageChannels[i], stageVolumes[i]);
        }
    }
    else {
        timer -= deltaTime;
    }*/

    currentStage = targetStage;
}

void MusicManager::StartGameTransition() {
    transitionActive = true;
    menuActive = false;

    auto* audio = ServiceLocator::getAudioEngine();
    transitionId = audio->PlayMusic(audio->musicPlay, GameManager::instance().sfxVolume * volume);

    //audio->stopSound(menuId);
}

void MusicManager::StartGameMusic()
{
	auto* audio = ServiceLocator::getAudioEngine();
	//baseId = audio->PlayMusic(audio->musicBase, GameManager::instance().sfxVolume * volume * 0.f);

    stageChannels[0] = audio->PlayMusic(audio->musicStage1, GameManager::instance().sfxVolume * volume);
    stageVolumes[0] = GameManager::instance().sfxVolume * volume * 0.3f;

    stageChannels[1] = audio->PlayMusic(audio->musicStage2, GameManager::instance().sfxVolume * 0.f);
    stageChannels[2] = audio->PlayMusic(audio->musicStage3, GameManager::instance().sfxVolume * 0.f);
    stageChannels[3] = audio->PlayMusic(audio->musicStage4, GameManager::instance().sfxVolume * 0.f);

    beatTime = 1.92f;
    timer = 0.f;
}

int MusicManager::getTargetStage(float distance, bool isInGas)
{
	if (isInGas || distance < 2.f) return 4;
	else if (distance < 8.f) return 3;
	else if (distance < 20.f) return 2;
	else return 1;
}

void MusicManager::PlayRewindSound() {
    auto* audio = ServiceLocator::getAudioEngine();

    for (int i = 0; i < 4; ++i) {
        stageVolumes[i] = 0.f;
        audio->SetChannelvolume(stageChannels[i], stageVolumes[i]);
    }

    rewindActive = true;
    rewindId = audio->PlayMusic(audio->musicRewind, GameManager::instance().sfxVolume * volume * 0.6f);
}

void MusicManager::StopRewindSound() {
    auto* audio = ServiceLocator::getAudioEngine();
    
    rewindActive = false;
    audio->stopSound(rewindId);
    audio->StopChannel(rewindId);
}

void MusicManager::onEnd() {
    menuId = -1;
    baseId = -1;
    transitionId = -1;

    stageChannels[0] = -1;
    stageChannels[1] = -1;
    stageChannels[2] = -1;
    stageChannels[3] = -1;

    //menuActive = false;

    beatTime = 1.92f;
    timer = 0.f;
}

