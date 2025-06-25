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

    //std::cout << "Distance to gas: " << distance << ", targetStage: " << targetStage << std::endl;

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

    if (gameMusicActive) {
        resyncTimer -= deltaTime;
        if (resyncTimer <= 0.0f) {
            resyncTimer = 1.0f; // Resetuj timer na nastêpn¹ sekundê

            FMOD::Channel* masterChannel = audio->GetChannel(stageChannels[0]);
            if (masterChannel) {
                // Pobierz pozycjê odtwarzania œcie¿ki-lidera w próbkach (najdok³adniej)
                unsigned int masterPosition = 0;
                masterChannel->getPosition(&masterPosition, FMOD_TIMEUNIT_MS);

                // PrzejdŸ przez pozosta³e œcie¿ki i ustaw ich pozycjê na tak¹ sam¹ jak u lidera
                for (int i = 1; i < 4; ++i) {
                    FMOD::Channel* followerChannel = audio->GetChannel(stageChannels[i]);
                    if (followerChannel) {
                        followerChannel->setPosition(masterPosition, FMOD_TIMEUNIT_MS);
                    }
                }
            }
        }
    }
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
    if (gameMusicActive) return;

    gameMusicActive = true;
    
    auto* audio = ServiceLocator::getAudioEngine();
    FMOD::System* fmodSystem = audio->GetLowLevelSystem();

    // Pobierz master channel group i utwórz (jeœli trzeba) w³asn¹ grupê muzyczn¹
    FMOD::ChannelGroup* masterGroup = nullptr;
    fmodSystem->getMasterChannelGroup(&masterGroup);
    if (!musicGroup) {
        fmodSystem->createChannelGroup("MusicGroup", &musicGroup);
        masterGroup->addGroup(musicGroup);
    }

    // Pobierz aktualny zegar DSP i sample rate
    unsigned long long dspClock = 0;
    masterGroup->getDSPClock(&dspClock, nullptr);
    int sampleRate = 0;
    fmodSystem->getSoftwareFormat(&sampleRate, nullptr, nullptr);

    // Ustal opóŸnienie startu np. 450ms
    unsigned long long delaySamples = static_cast<unsigned long long>(sampleRate * 0.45f);
    unsigned long long startDelay = dspClock + delaySamples;

    const std::string stageNames[4] = {
        audio->musicStage1,
        audio->musicStage2,
        audio->musicStage3,
        audio->musicStage4
    };

    float baseVol = GameManager::instance().sfxVolume * volume * 0.3f;
    float initialVolumes[4] = { baseVol, 0.f, 0.f, 0.f };

    // Odtwarzaj ka¿dy kana³ w paused, ustawiaj¹c delay i g³oœnoœæ, potem unpause
    for (int i = 0; i < 4; ++i) {
        FMOD::Sound* sound = audio->GetSoundByName(stageNames[i]);
        if (!sound) {
            audio->LoadSound(stageNames[i], false, true, false);
            sound = audio->GetSoundByName(stageNames[i]);
        }

        FMOD::Channel* channel = nullptr;
        fmodSystem->playSound(sound, nullptr, true, &channel);
        if (!channel) continue;

        channel->setChannelGroup(musicGroup);
        // ustaw liniowy poziom g³oœnoœci [0..1]
        channel->setVolume(glm::clamp(initialVolumes[i], 0.f, 1.f));

        // ustaw sample-dok³adne opóŸnienie startu
        channel->setDelay(startDelay, 0, true);

        // odblokuj kana³
        channel->setPaused(false);

        // zarejestruj kana³
        stageChannels[i] = audio->generateChannelId();
        audio->RegisterChannel(stageChannels[i], channel);
        stageVolumes[i] = initialVolumes[i];
    }

    // Nie ma ju¿ potrzeby pausing/ unpausing grupy — kana³y same wystartowa³y.
    beatTime = 1.92f;
    timer = 0.f;

    //if (gameMusicActive) return;

    //gameMusicActive = true;
    //auto* audio = ServiceLocator::getAudioEngine();

    //// 1. Przygotuj listê œcie¿ek do odtworzenia
    //std::vector<std::string> tracksToPlay = {
    //    audio->musicStage1,
    //    audio->musicStage2,
    //    audio->musicStage3,
    //    audio->musicStage4
    //};

    //// 2. Wywo³aj funkcjê silnika, która przygotuje kana³y (w stanie pauzy)
    //std::vector<int> newChannelIds = audio->PlayMusicTracks(tracksToPlay);

    //// 3. Wywo³aj funkcjê silnika, która uruchomi je wszystkie idealnie w tym samym momencie
    //audio->StartMusicGroup();

    //// 4. Przypisz zwrócone ID kana³ów do lokalnych zmiennych
    //if (newChannelIds.size() == 4) {
    //    for (int i = 0; i < 4; ++i) {
    //        stageChannels[i] = newChannelIds[i];
    //    }
    //}

    //// 5. Ustaw g³oœnoœci pocz¹tkowe (wszystkie na 0, a potem pêtla Update zajmie siê reszt¹)
    //for (int i = 0; i < 4; ++i) {
    //    stageVolumes[i] = 0.f; // Zacznij z cisz¹
    //    if (stageChannels[i] != -1) {
    //        audio->SetChannelvolume(stageChannels[i], 0.f);
    //    }
    //}

    //for (int i = 0; i < 4; ++i) {
    //    stageVolumes[i] = 100.f; // <- zamiast 0.f
    //    if (stageChannels[i] != -1) {
    //        audio->SetChannelvolume(stageChannels[i], 100.f);
    //    }
    //}
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
    rewindId = audio->PlayMusic(audio->musicRewind, GameManager::instance().musicVolume * volume);
}

void MusicManager::StopRewindSound() {
    auto* audio = ServiceLocator::getAudioEngine();
    
    rewindActive = false;
    audio->stopSound(rewindId);
    audio->StopChannel(rewindId);
}

void MusicManager::PlayDeathMusic() {
    auto* audio = ServiceLocator::getAudioEngine();

    if (menuActive) {
        audio->stopSound(menuId);
        menuActive = false;
    }

    deathId = audio->PlayMusic(audio->musicDeath, GameManager::instance().musicVolume * volume);
    deathActive = true;
}

void MusicManager::StopDeathMusic() {
    auto* audio = ServiceLocator::getAudioEngine();

    if (deathActive) {
        audio->stopSound(deathId);
        deathActive = false;
    }

    menuId = audio->PlayMusic(audio->musicMenu, GameManager::instance().musicVolume * volume);
    menuActive = true;
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

    gameMusicActive = false;

    beatTime = 1.92f;
    timer = 0.f;
}

