#include "AudioEngine.h"

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
        it->second->isPlaying(&bIsPlaying);
        if (!bIsPlaying)
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

