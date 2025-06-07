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

    std::string sound1 = "res/sounds/bonk.ogg";

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
    void PlayEvent(const string& strEventName);
    void StopChannel(int nChannelId);
    void StopEvent(const string& strEventName, bool bImmediate = false);
    //void GetEventParameter(const string& strEventName, const string& strEventParameter, float* parameter);
    //void SetEventParameter(const string& strEventName, const string& strParameterName, float fValue);
    //void StopAllChannels();
    void SetChannel3dPosition(int nChannelId, const glm::vec3& vPosition);
    void SetChannelvolume(int nChannelId, float fVolumedB);
    //bool IsPlaying(int nChannelId) const;
    bool IsEventPlaying(const string& strEventName) const;
    float percentToDb(float percent);
    float dbToVolume(float db);
    float VolumeTodB(float volume);
    FMOD_VECTOR VectorToFmod(const Vector3& vPosition);
    FMOD_VECTOR glmToFmod(const glm::vec3& vPosition);

    void loadAllGameSounds();
    
    // Not from tutorial so potentially some real garbage code
    void stopSound(int nChannelId);
    void pauseSound(int nChannelId);
    void resumeSound(int nChannelId);

};

#endif // !AUDIO_ENGINE_H
