#include "AudioEngine.h"
#include <glm/common.hpp>

// ---
//      ErrorCheck checks if all FMOD calls were successful
// ---

Implementation::Implementation() {
    mpStudioSystem = NULL;
    // Fmod Studio System handles all events and sounds
    CAudioEngine::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
    // Initializing, 32 = number of channels, and some flags
    CAudioEngine::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

    mpSystem = NULL;
    // We create low level system which handles all the low level stuff
    CAudioEngine::ErrorCheck(mpStudioSystem->getCoreSystem(&mpSystem));
}

Implementation::~Implementation() {
    // Unloads all assets
    CAudioEngine::ErrorCheck(mpStudioSystem->unloadAll());
    // Shuts down the systems
    CAudioEngine::ErrorCheck(mpStudioSystem->release());
}

void Implementation::Update() {
    vector<ChannelMap::iterator> pStoppedChannels;
    // Here we check all the channels and if it has, we destroy it so we can clear the channel and use it for another sound
    for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
    {
        bool bIsPlaying = false;
        bool bIsPaused = false;

        it->second->isPlaying(&bIsPlaying);
        it->second->getPaused(&bIsPaused);

        if (!bIsPlaying && !bIsPaused)
        {
            pStoppedChannels.push_back(it);
        }
    }
    for (auto& it : pStoppedChannels)
    {
        mChannels.erase(it);
    }
    // We call the update of the system
    CAudioEngine::ErrorCheck(mpStudioSystem->update());
    CAudioEngine::ErrorCheck(mpSystem->update());
}

Implementation* sgpImplementation = nullptr;

void CAudioEngine::Init()
{
    // Creates Implementation structure
    sgpImplementation = new Implementation;
}

void CAudioEngine::Update()
{
    // Calls update from the Implementation structure
    sgpImplementation->Update();
}

void CAudioEngine::Shutdown()
{
    // Deletes Implementation (calls destructor)
    delete sgpImplementation;
}

int CAudioEngine::ErrorCheck(FMOD_RESULT result)
{
    // Handles error checking (see if result of the call is FMOD_OK)
    if(result != FMOD_OK) {
        cout << "FMOD ERROR " << result << endl;
        return 1;
    }
    return 0;
}

void CAudioEngine::LoadBank(const string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
    // Find bank by name
    auto tFoundIt = sgpImplementation->mBanks.find(strBankName);
    // If it is already loaded we don't load it again
    if (tFoundIt != sgpImplementation->mBanks.end()) { return; }
    
    // Create a bank object
    FMOD::Studio::Bank* pBank;
    // Load it
    CAudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
    if (pBank) {
        // Add it to the bank map
        sgpImplementation->mBanks[strBankName] = pBank;
    }
}

void CAudioEngine::LoadEvent(const string& strEventName)
{
    // Find event by name
    auto tFoundit = sgpImplementation->mEvents.find(strEventName);
    // If it is already loaded we don't load it again
    if (tFoundit != sgpImplementation->mEvents.end()) { return; }

    // First we add description (covers all the info), and then instance (what actually plays the sound)
    FMOD::Studio::EventDescription* pEventDescription = NULL;
    CAudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
    if (pEventDescription) {
        FMOD::Studio::EventInstance* pEventInstance = NULL;
        CAudioEngine::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
        if (pEventInstance) {
            // Add event to the event map
            sgpImplementation->mEvents[strEventName] = pEventInstance;
        }
    }
}

void CAudioEngine::LoadSound(const string& strSoundName, bool b3d, bool bLooping, bool bStream)
{
    // We find a sound by file name
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    // If it is already loaded we don't load it again
    if (tFoundIt != sgpImplementation->mSounds.end()) { return; }

    // By ckecking passed bool flags we check if:
    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= b3d ? FMOD_3D : FMOD_2D; // The sound is 3D
    eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF; // The sound should play in a loop
    eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE; // The sound is streamed

    // Here we just create the sound object
    FMOD::Sound* pSound = nullptr;
    CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
    if (pSound) {
        // We save it in a map with a key of the file name
        sgpImplementation->mSounds[strSoundName] = pSound;
    }
}

void CAudioEngine::UnloadSound(const string& strSoundName)
{
    // Find the sound by file name
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    // If it isn't loaded we don't try to unload it
    if (tFoundIt == sgpImplementation->mSounds.end()) { return; }

    // Release memory of the sound
    CAudioEngine::ErrorCheck(tFoundIt->second->release());
    // Delete a pointer from the sound map
    sgpImplementation->mSounds.erase(tFoundIt);
}

void CAudioEngine::Set3dListenerAndOrientation(const glm::vec3& vPos, const glm::vec3& vVel, const glm::vec3& vFor, const glm::vec3& vUp) {
    FMOD_VECTOR fmodPos = glmToFmod(vPos);
    FMOD_VECTOR fmodVel = glmToFmod(vVel);
    FMOD_VECTOR fmodForward = glmToFmod(vFor);
    FMOD_VECTOR fmodUp = glmToFmod(vUp);

    sgpImplementation->mpSystem->set3DListenerAttributes(0, &fmodPos, &fmodVel, &fmodForward, &fmodUp);
}

int CAudioEngine::PlayMusic(const string& strSoundName, float volumePercent, const glm::vec3& vPosition)
{
    // Set channel id
    int nChannelId = sgpImplementation->mnNextChannelId++;
    // Try to find sound in the sound map. Load it if it's not there
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->mSounds.end())
    {
        LoadSound(strSoundName);
        tFoundIt = sgpImplementation->mSounds.find(strSoundName);
        // This means we still can't find the sound so we won't play it. Just exit the method
        if (tFoundIt == sgpImplementation->mSounds.end())
        {
            // Return channel id so we can refer to it later (? Why if it's not even there ?)
            return nChannelId;
        }
    }
    // Create the channel
    FMOD::Channel* pChannel = nullptr;
    // Play sound
    CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
    if (pChannel) {
        FMOD_MODE currMode;
        tFoundIt->second->getMode(&currMode);
        if (currMode & FMOD_3D) {
            // If the sound is 3D we set the position
            FMOD_VECTOR position = glmToFmod(vPosition);
            CAudioEngine::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
        }
        // Other settings (volume and paused - unpause)
        float dB_from_percent = percentToDb(volumePercent);
        CAudioEngine::ErrorCheck(pChannel->setVolume(dbToVolume(dB_from_percent)));
        CAudioEngine::ErrorCheck(pChannel->setPaused(false));
        // Add channel to channel map
        sgpImplementation->mChannels[nChannelId] = pChannel;
    }
    // And return the id so we can refer to it later
    return nChannelId;
}

void CAudioEngine::PlaySFX(const string& strSoundName, float volumePercent, const glm::vec3& vPosition) {
    int temp;
    temp = PlayMusic(strSoundName, volumePercent, vPosition);
}



void CAudioEngine::PlayEvent(const string& strEventName)
{
    auto tFoundit = sgpImplementation->mEvents.find(strEventName);
    if (tFoundit == sgpImplementation->mEvents.end()) {
        // Load event if it has not yet been loaded
        LoadEvent(strEventName);
        tFoundit = sgpImplementation->mEvents.find(strEventName);
        if (tFoundit == sgpImplementation->mEvents.end())
            // If we still can't find it. something went wrong - exit method
            return;
    }
    // Playing the event
    tFoundit->second->start();
}

void CAudioEngine::StopChannel(int nChannelId)
{

}

void CAudioEngine::StopEvent(const string& strEventName, bool bImmediate)
{
    // Find event by name
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end()) { return; }

    // How we stop our sound - instantly or with fadeout
    FMOD_STUDIO_STOP_MODE eMode;
    eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
    // Call stopping the event
    CAudioEngine::ErrorCheck(tFoundIt->second->stop(eMode));
}

//void CAudioEngine::GetEventParameter(const string& strEventName, const string& strEventParameter, float* parameter)
//{
//    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
//    if (tFoundIt == sgpImplementation->mEvents.end())
//        return;
//    FMOD::Studio::ParameterInstance* pParameter = NULL;
//    CAudioEngine::ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
//    CAudioEngine::ErrorCheck(pParameter->getValue(parameter));
//}

void CAudioEngine::SetChannel3dPosition(int nChannelId, const glm::vec3& vPosition)
{
    // Find channel by id
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end()) { return; }

    // Make an Fmod vector from our struct Vector3
    FMOD_VECTOR position = glmToFmod(vPosition);
    // We set position (velocity as null)
    CAudioEngine::ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
}

void CAudioEngine::SetChannel3dMinMaxDistance(int nChannelId, float minDistance, float maxDistance)
{
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end()) {
		std::cout << "CAudioEngine::SetChannel3dMinMaxDistance: Channel with id " << nChannelId << " not found." << std::endl;
        return;
    }

    CAudioEngine::ErrorCheck(tFoundIt->second->setMode(FMOD_3D_LINEARROLLOFF));
    CAudioEngine::ErrorCheck(tFoundIt->second->set3DMinMaxDistance(minDistance, maxDistance));
}

void CAudioEngine::SetChannelvolume(int nChannelId, float fVolumedB)
{
    // Find channel by id
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end()) { return; }

    // Set volume of the channel
    //CAudioEngine::ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));

    float volumeLinear = glm::clamp(fVolumedB, 0.f, 100.f) / 100.f;
    bool isPlaying = false;
    tFoundIt->second->isPlaying(&isPlaying);
    if (isPlaying) {
        CAudioEngine::ErrorCheck(tFoundIt->second->setVolume(volumeLinear));
    }
}

bool CAudioEngine::IsEventPlaying(const string& strEventName) const
{
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return false;

    // This playback tells us if the event is playing or no
    FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
    if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) {
        return true;
    }
    return false;
}

bool CAudioEngine::IsPlaying(int nChannelId) const
{
    auto it = sgpImplementation->mChannels.find(nChannelId);
    if (it != sgpImplementation->mChannels.end())
    {
        bool isPlaying = false;
        if (it->second)
        {
            FMOD_RESULT result = it->second->isPlaying(&isPlaying);
            if (result == FMOD_OK)
                return isPlaying;
        }
    }
    return false;
}

FMOD_VECTOR CAudioEngine::VectorToFmod(const Vector3& vPosition)
{
    // Make an Fmod vector from our struct Vector3
    FMOD_VECTOR fVec;
    fVec.x = vPosition.x;
    fVec.y = vPosition.y;
    fVec.z = vPosition.z;
    return fVec;
}

FMOD_VECTOR CAudioEngine::glmToFmod(const glm::vec3& vPosition)
{
    FMOD_VECTOR fVec;
    fVec.x = vPosition.x;
    fVec.y = vPosition.y;
    fVec.z = vPosition.z;
    return fVec;
}

/*
//  This should be done when we don't want to play the sound anytime soon as it deletes it from the map.
//  I believe finding something in a map is faster than deleting it and then adding once more. And so on...
*/

void CAudioEngine::stopSound(int nChannelId)
{
    // Find the channel by id
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end()) { return; }

    // Stop the sound immediately
    CAudioEngine::ErrorCheck(tFoundIt->second->stop());

    // Remove the channel from the map
    sgpImplementation->mChannels.erase(tFoundIt);
}

/*
//  This may be useful for music or longer sounds
*/

void CAudioEngine::pauseSound(int nChannelId)
{
    // Find the channel by id
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end()) { return; }

    // Pauses the sound, keeping it in the map
    CAudioEngine::ErrorCheck(tFoundIt->second->setPaused(true));
}

void CAudioEngine::resumeSound(int nChannelId)
{
    // Find the channel by id
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end()) { return; }

    // Unpauses the sound
    CAudioEngine::ErrorCheck(tFoundIt->second->setPaused(false));
}

float CAudioEngine::percentToDb(float percent)
{
    float silence = -80.0f;
    float max_vol = 0.0f;
    if (percent <= 0.0f) {
        return silence;
    }
    else if (percent >= 100.0f) {
        return max_vol;
    }
    else {
        return silence + (percent / 100.0f) * 80.0f;
    }
}

float CAudioEngine::dbToVolume(float dB)
{
    return powf(10.0f, 0.05f * dB);
}

float CAudioEngine::VolumeTodB(float volume)
{
    return 20.0f * log10f(volume);
}

void CAudioEngine::loadAllGameSounds() {
    LoadSound(sound1, false, false, false);
	LoadSound(button_down, false, false, false);
	LoadSound(button_up, false, false, false);
	LoadSound(gate_open, true, false, false);
	LoadSound(activation, true, false, false);
	LoadSound(electricity, true, true, false);
	LoadSound(electrified, false, false, false);
	LoadSound(jumping, false, false, false);
	LoadSound(landing, false, false, false);
	LoadSound(laser_hit, true, true, false);
	LoadSound(pushing, false, false, false);
	LoadSound(wind_blow, false, false, false);
	LoadSound(fan, true, true, false);
	LoadSound(death, false, false, false);
	LoadSound(death_spikes, false, false, false);
	LoadSound(running, false, true, false);
	LoadSound(eating, false, false, false);
	LoadSound(writing, false, false, false);
	LoadSound(propeller, true, true, false);
	LoadSound(running_step, false, false, false);
	LoadSound(rewind, false, false, false);

    // music
    LoadSound(musicMenu, false, true, false);
    LoadSound(musicPlay, false, true, false);
    LoadSound(musicPause, false, true, false);
    LoadSound(musicBase, false, true, false);
    LoadSound(musicStage1, false, true, false);
    LoadSound(musicStage2, false, true, false);
    LoadSound(musicStage3, false, true, false);
    LoadSound(musicStage4, false, true, false);
}

void CAudioEngine::unloadAllGameSounds() {

    UnloadSound(sound1);
    UnloadSound(button_down);
    UnloadSound(button_up);
    UnloadSound(gate_open);
    UnloadSound(activation);
    UnloadSound(electricity);
    UnloadSound(electrified);
    UnloadSound(jumping);
    UnloadSound(landing);
    UnloadSound(laser_hit);
    UnloadSound(pushing);
    UnloadSound(wind_blow);
    UnloadSound(fan);
    UnloadSound(death);
    UnloadSound(death_spikes);
    UnloadSound(running);
    UnloadSound(eating);
    UnloadSound(writing);
    UnloadSound(propeller);
    UnloadSound(running_step);
    UnloadSound(rewind);

    // music
    UnloadSound(musicMenu);
    UnloadSound(musicPlay);
    UnloadSound(musicPause);
    UnloadSound(musicBase);
    UnloadSound(musicStage1);
    UnloadSound(musicStage2);
    UnloadSound(musicStage3);
    UnloadSound(musicStage4);
}
