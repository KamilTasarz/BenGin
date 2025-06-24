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
        transitionTimer -= deltaTime;

        if (audio->IsPlaying(menuId)) {
            menuVolume = glm::mix(menuVolume, 0.f, deltaTime * 2.f);
            audio->SetChannelvolume(menuId, menuVolume);
        }
        else {
            audio->stopSound(menuId);
        }

        if (transitionTimer <= 0.f) {
            StartGameMusic();
            transitionActive = false;
        }
        return;
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

    FMOD::Sound* transitionSound = audio->GetSoundByName(audio->musicPlay);
    if (transitionSound) {
        unsigned int lengthMs = 0;
        transitionSound->getLength(&lengthMs, FMOD_TIMEUNIT_MS);

        transitionTimer = (lengthMs - 500) / 1000.0f;
    }
}

void MusicManager::StartGameMusic()
{
    auto* audio = ServiceLocator::getAudioEngine();
    FMOD::System* fmodSystem = audio->GetLowLevelSystem();

    // Pobierz g³ówn¹ grupê kana³ów
    FMOD::ChannelGroup* masterGroup = nullptr;
    fmodSystem->getMasterChannelGroup(&masterGroup);

    // Utwórz wspóln¹ grupê dla muzyki (opcjonalnie zachowaj jako zmienn¹ cz³onkowsk¹)
    musicGroup = nullptr;
    fmodSystem->createChannelGroup("MusicGroup", &musicGroup);

    // Pobierz aktualny zegar DSP
    unsigned long long dspClock = 0;
    masterGroup->getDSPClock(&dspClock, nullptr);

    // Pobierz sample rate
    int sampleRate = 0;
    fmodSystem->getSoftwareFormat(&sampleRate, nullptr, nullptr);

    // Delay ~450ms (mo¿esz zwiêkszyæ do np. 700ms jeœli desync nadal siê zdarza)
    unsigned long long delaySamples = static_cast<unsigned long long>(sampleRate * 0.45f);
    unsigned long long startDelay = dspClock + delaySamples;

    const std::string stageNames[4] = {
        audio->musicStage1,
        audio->musicStage2,
        audio->musicStage3,
        audio->musicStage4
    };

    float initialVolumes[4] = {
        GameManager::instance().sfxVolume * volume * 0.3f, // tylko stage1 aktywny na start
        0.f,
        0.f,
        0.f
    };

    for (int i = 0; i < 4; ++i) {
        FMOD::Sound* sound = audio->GetSoundByName(stageNames[i]);
        if (!sound) {
            audio->LoadSound(stageNames[i], false, true, false);
            sound = audio->GetSoundByName(stageNames[i]);
        }

        FMOD::Channel* channel = nullptr;
        fmodSystem->playSound(sound, nullptr, true, &channel);

        if (channel) {
            // Ustaw kana³ w grupie muzyki
            channel->setChannelGroup(musicGroup);

            // Jeœli kana³ ma byæ "wyciszony" - u¿yj mute
            bool isMuted = (initialVolumes[i] <= 0.001f);
            //channel->setMute(isMuted);
            channel->setVolume(initialVolumes[i] / 100.f);

            // Ustaw wspólny delay dla synchronizacji
            channel->setDelay(startDelay, 0, true);

            // Start odtwarzania
            channel->setPaused(false);

            // Zarejestruj kana³ w Twoim systemie
            stageChannels[i] = audio->generateChannelId();
            audio->RegisterChannel(stageChannels[i], channel);
            stageVolumes[i] = initialVolumes[i];
        }
    }

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

    if (musicGroup) {
        musicGroup->release();
        musicGroup = nullptr;
    }

    beatTime = 1.92f;
    timer = 0.f;
}

