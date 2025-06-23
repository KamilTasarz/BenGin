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
	auto* audio = ServiceLocator::getAudioEngine();
	menuId = audio->PlayMusic(audio->musicMenu, GameManager::instance().sfxVolume * 80.f);

	menuActive = true;
}

//void MusicManager::Update(float deltaTime)
//{
//	auto* audio = ServiceLocator::getAudioEngine();
//	Node* player = GameManager::instance().currentPlayer;
//
//	if (!player) {
//		return;
//	}
//	else if (player && menuActive) {
//		menuActive = false;
//
//		audio->PlaySFX(audio->musicPlay, GameManager::instance().sfxVolume * 80.f);
//
//		StartGameMusic();
//	}
//
//	float fadeSpeed = 1.f;
//	float targetVolume = GameManager::instance().sfxVolume * 80.f;
//
//	float distFromPlayerToGas = GameManager::instance().minDistance;
//	bool isInGas = player->getComponent<PlayerController>()->isInGas;
//
//	float targetVolumeTwo = (distFromPlayerToGas < 25.f) ? targetVolume : 0.f;
//	stageTwoVolume = glm::mix(stageTwoVolume, targetVolumeTwo, deltaTime / fadeSpeed);
//	audio->SetChannelvolume(stageTwoId, stageTwoVolume);
//
//	float targetVolumeThree = (distFromPlayerToGas < 10.f) ? targetVolume : 0.f;
//	stageThreeVolume = glm::mix(stageThreeVolume, targetVolumeThree, deltaTime / fadeSpeed);
//	audio->SetChannelvolume(stageThreeId, stageThreeVolume);
//
//	float targetVolumeFour = (distFromPlayerToGas < 2.f || isInGas) ? targetVolume : 0.f;
//	stageFourVolume = glm::mix(stageFourVolume, targetVolumeFour, deltaTime / fadeSpeed);
//	audio->SetChannelvolume(stageFourId, stageFourVolume);
//}

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
            return;
        }
        else {
            StartGameMusic();
            transitionActive = false;
        }
    }

    float fadeSpeed = .5f;
    float targetVolume = GameManager::instance().sfxVolume * 85.f;

    float distance = GameManager::instance().minPlayerToParticleDistance;
    bool isInGas = player->getComponent<PlayerController>()->isInGas;

    int targetStage = getTargetStage(distance, isInGas);

    std::cout << "Distance to gas: " << distance << ", targetStage: " << targetStage << std::endl;

    // plynna zmiana

    for (int i = 0; i < 4; ++i) {
        float desiredVolume = (i + 1 == targetStage) ? targetVolume : 0.f;
        stageVolumes[i] = glm::mix(stageVolumes[i], desiredVolume, deltaTime / fadeSpeed);
        audio->SetChannelvolume(stageChannels[i], stageVolumes[i]);
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
    transitionId = audio->PlayMusic(audio->musicBase, GameManager::instance().sfxVolume * 85.f);

    audio->stopSound(menuId);
}

void MusicManager::StartGameMusic()
{
	auto* audio = ServiceLocator::getAudioEngine();
	baseId = audio->PlayMusic(audio->musicBase, GameManager::instance().sfxVolume * 80.f);

    stageChannels[0] = audio->PlayMusic(audio->musicStage1, GameManager::instance().sfxVolume * 85.f);
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

void MusicManager::onEnd() {
    menuId = -1;
    baseId = -1;
    transitionId = -1;

    stageChannels[0] = -1;
    stageChannels[1] = -1;
    stageChannels[2] = -1;
    stageChannels[3] = -1;

    menuActive = false;

    beatTime = 1.92f;
    timer = 0.f;
}

