#include "RewindManager.h"

RewindManager& RewindManager::Instance() {
    static RewindManager instance;
    return instance;
}

void RewindManager::registerRewindable(TimeRewindable* rewindable) {
    if (rewindable && std::find(rewindables.begin(), rewindables.end(), rewindable) == rewindables.end()) {
        rewindables.push_back(rewindable);
    }
}

void RewindManager::unregisterRewindable(TimeRewindable* rewindable) {
    rewindables.erase(std::remove(rewindables.begin(), rewindables.end(), rewindable), rewindables.end());
}

void RewindManager::resetAllHistories() {
    for (auto* rewindable : rewindables) {
        if (rewindable) rewindable->resetHistory();
    }
}
