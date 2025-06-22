#include "MusicManager.h"
#include "../AudioEngine.h"
#include "../System/ServiceLocator.h"
#include "GameManager.h"
#include "PlayerController.h"

void MusicManager::Init()
{
	auto* audio = ServiceLocator::getAudioEngine();
	//menuId = audio->PlayMusic(audio->menuMusic, GameManager::instance().sfxVolume * 80.f);

	menuActive = true;
}

void MusicManager::Update(float deltaTime)
{
	auto* audio = ServiceLocator::getAudioEngine();
	Node* player = GameManager::instance().currentPlayer;

	if (!player) {
		return;
	}
	else if (player && menuActive) {
		menuActive = false;

		//audio->PlaySFX(audio->StartRunMusic, GameManager::instance().sfxVolume * 80.f);

		StartGameMusic();
	}

	/*float fadeSpeed = 1.5f;
	float targetVolume = GameManager::instance().sfxVolume * 80.f;

	float distFromPlayerToGas = GameManager::instance().minDistance;
	bool isInGas = player->getComponent<PlayerController>()->isInGas;

	float targetVolumeTwo = (distFromPlayerToGas < 25.f) ? targetVolume : 0.f;
	stageTwoVolume = glm::mix(stageTwoVolume, targetVolumeTwo, deltaTime / fadeSpeed);
	audio->SetChannelvolume(stageTwoId, stageTwoVolume);

	float targetVolumeThree = (distFromPlayerToGas < 10.f) ? targetVolume : 0.f;
	stageThreeVolume = glm::mix(stageThreeVolume, targetVolumeThree, deltaTime / fadeSpeed);
	audio->SetChannelvolume(stageThreeId, stageThreeVolume);

	float targetVolumeFour = (distFromPlayerToGas < 2.f || isInGas) ? targetVolume : 0.f;
	stageFourVolume = glm::mix(stageFourVolume, targetVolumeFour, deltaTime / fadeSpeed);
	audio->SetChannelvolume(stageFourId, stageFourVolume);*/
}

void MusicManager::StartGameMusic()
{
	auto* audio = ServiceLocator::getAudioEngine();
	//stageOneId = audio->PlayMusic(audio->menuMusic, GameManager::instance().sfxVolume * 80.f);
	//stageTwoId = audio->PlayMusic(audio->menuMusic, GameManager::instance().sfxVolume * 0.f);
	//stageThreeId = audio->PlayMusic(audio->menuMusic, GameManager::instance().sfxVolume * 0.f);
	//stageFourId = audio->PlayMusic(audio->menuMusic, GameManager::instance().sfxVolume * 0.f);

	stageOneActive = true;
}
