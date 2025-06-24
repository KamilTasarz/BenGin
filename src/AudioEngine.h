#pragma once

#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <fmod/fmod.hpp>
#include <fmod_studio/fmod_studio.hpp>

#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include <glm/vec3.hpp>

using namespace std;

struct Vector3 {
    float x;
    float y;
    float z;
};

struct Implementation {

    Implementation();
    ~Implementation();

    void Update();

    FMOD::Studio::System* mpStudioSystem;
    FMOD::System* mpSystem;

    int mnNextChannelId;

    typedef map<string, FMOD::Sound*> SoundMap;
    typedef map<int, FMOD::Channel*> ChannelMap;
    typedef map<string, FMOD::Studio::EventInstance*> EventMap;
    typedef map<string, FMOD::Studio::Bank*> BankMap;

    BankMap mBanks;
    EventMap mEvents;
    SoundMap mSounds;
    ChannelMap mChannels;

};

class CAudioEngine {

public:
    // sfx
    std::string sound1 = "res/audios/sounds/bonk.ogg";
    std::string button_down = "res/audios/sounds/button_down.ogg";
    std::string button_up = "res/audios/sounds/button_up.ogg";
	std::string gate_open = "res/audios/sounds/gate_open.ogg";
	//std::string gate_close = "res/audios/sounds/gate_close.mp3";
	std::string activation = "res/audios/sounds/activation.ogg";
	std::string electricity = "res/audios/sounds/electricity.ogg";
	std::string electrified = "res/audios/sounds/electrified.ogg";
	std::string jumping = "res/audios/sounds/jumping.ogg";
	std::string landing = "res/audios/sounds/landing.ogg";
	std::string laser_hit = "res/audios/sounds/laser_hit.ogg";
	std::string pushing = "res/audios/sounds/pushing.ogg";
	std::string wind_blow = "res/audios/sounds/wind_blow.ogg";
	std::string fan = "res/audios/sounds/fan.ogg";
	std::string death = "res/audios/sounds/death.ogg";
	std::string death_spikes = "res/audios/sounds/spikes_death.ogg";
	std::string running = "res/audios/sounds/running.ogg";
	std::string eating = "res/audios/sounds/wpierdalanie_sera.ogg";
	std::string writing = "res/audios/sounds/writing.ogg";
	std::string propeller = "res/audios/sounds/tv_propeller.ogg";
    std::string running_step = "res/audios/sounds/running_step.ogg";
    std::string rewind = "res/audios/sounds/rewind.ogg";

    //music
    std::string musicMenu = "res/audios/music/musicMenu.mp3";
    std::string musicPlay = "res/audios/music/musicPlay.mp3";
    std::string musicPause = "res/audios/music/musicPause.mp3";
    std::string musicBase = "res/audios/music/musicBase.mp3";
    std::string musicStage1 = "res/audios/music/musicStage1.mp3";
    std::string musicStage2 = "res/audios/music/musicStage2.mp3";
    std::string musicStage3 = "res/audios/music/musicStage3.mp3";
    std::string musicStage4 = "res/audios/music/musicStage4.mp3";
    std::string musicRewind = "res/audios/music/musicRewind.mp3";

    static void Init();
    static void Update();
    static void Shutdown();
    static int ErrorCheck(FMOD_RESULT result);

    void LoadBank(const string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
    void LoadEvent(const string& strEventName);
    void LoadSound(const string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
    void UnloadSound(const string& strSoundName);
    void Set3dListenerAndOrientation(const glm::vec3& vPos, const glm::vec3& vVel, const glm::vec3& vFor, const glm::vec3& vUp);
    int PlayMusic(const string& strSoundName, float volumePercent = 100.0f, const glm::vec3& vPosition = glm::vec3{ 0, 0, 0 });
    void PlaySFX(const string& strSoundName, float volumePercent = 100.0f, const glm::vec3& vPosition = glm::vec3{ 0, 0, 0 });
    void PlaySFXWithFadeIn(const string& strSoundName, float volumePercent, const glm::vec3& vPosition, float fadeTime);
    void PlayEvent(const string& strEventName);
    void StopChannel(int nChannelId);
    void StopEvent(const string& strEventName, bool bImmediate = false);
    //void GetEventParameter(const string& strEventName, const string& strEventParameter, float* parameter);
    //void SetEventParameter(const string& strEventName, const string& strParameterName, float fValue);
    //void StopAllChannels();
    void SetChannel3dPosition(int nChannelId, const glm::vec3& vPosition);
    void SetChannel3dMinMaxDistance(int nChannelId, float minDistance, float maxDistance);
    void SetChannelvolume(int nChannelId, float fVolumedB);
    //bool IsPlaying(int nChannelId) const;
    bool IsEventPlaying(const string& strEventName) const;
    bool IsPlaying(int nChannelId) const;
    float percentToDb(float percent);
    float dbToVolume(float db);
    float VolumeTodB(float volume);
    FMOD_VECTOR VectorToFmod(const Vector3& vPosition);
    FMOD_VECTOR glmToFmod(const glm::vec3& vPosition);

    void loadAllGameSounds();
    void unloadAllGameSounds();
    
    // Not from tutorial so potentially some real garbage code
    void stopSound(int nChannelId);
    void pauseSound(int nChannelId);
    void resumeSound(int nChannelId);

    std::vector<int> PlayMusicTracks(const std::vector<std::string>& trackNames);

    void StartMusicGroup();

    FMOD::System* GetLowLevelSystem();

    int generateChannelId();

    FMOD::Sound* GetSoundByName(const std::string& name);

    void RegisterChannel(int channelId, FMOD::Channel* channel);

};

#endif // !AUDIO_ENGINE_H
