#pragma once

#include <vector>
#include <memory>
#include "TimeRewindable.h"

class RewindManager {
public:
    static RewindManager& Instance();

    void registerRewindable(TimeRewindable* rewindable);
    void unregisterRewindable(TimeRewindable* rewindable);

    void resetAllHistories();

private:
    std::vector<TimeRewindable*> rewindables;
};
