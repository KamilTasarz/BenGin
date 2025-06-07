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

    static void Init();
    static void Update();
    static void Shutdown();
    static int ErrorCheck(FMOD_RESULT result);

    void LoadBank(const string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
    void LoadEvent(const string& strEventName);
    void LoadSound(const string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
    void UnloadSound(const string& strSoundName);
    //void Set3dListenerAndOrientation(const Vector3& vPos = Vector3{ 0, 0, 0 }, float fVolumedB = 0.0f);
    int PlaySounds(const string& strSoundName, const Vector3& vPosition = Vector3{ 0, 0, 0 }, float volumePercent = 0.0f);
    void PlayEvent(const string& strEventName);
    void StopChannel(int nChannelId);
    void StopEvent(const string& strEventName, bool bImmediate = false);
    //void GetEventParameter(const string& strEventName, const string& strEventParameter, float* parameter);
    //void SetEventParameter(const string& strEventName, const string& strParameterName, float fValue);
    //void StopAllChannels();
    void SetChannel3dPosition(int nChannelId, const Vector3& vPosition);
    void SetChannelvolume(int nChannelId, float fVolumedB);
    //bool IsPlaying(int nChannelId) const;
    bool IsEventPlaying(const string& strEventName) const;
    float percentToDb(float percent);
    float dbToVolume(float db);
    float VolumeTodB(float volume);
    FMOD_VECTOR VectorToFmod(const Vector3& vPosition);

    // Not from tutorial so potentially some real garbage code
    void stopSound(int nChannelId);
    void pauseSound(int nChannelId);
    void resumeSound(int nChannelId);

};

#endif // !AUDIO_ENGINE_H
