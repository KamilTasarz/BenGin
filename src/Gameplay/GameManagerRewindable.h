#pragma once

#include "TimeRewindable.h"
#include <memory>

class GameManager;

struct GameManagerSnapshot : public ITimeSnapshot {
    float runTime = 0.0f;
    int deathCount = 0;
    float score = 0.0f;
};

class GameManagerRewindable : public TimeRewindable {
public:
    GameManager* gameManager = nullptr;

    void onAttach(Node* owner) override;
    void onDetach() override;
    void onStart() override;

protected:
    std::shared_ptr<ITimeSnapshot> createSnapshot() override;
    void applySnapshot(const std::shared_ptr<ITimeSnapshot>& snapshot) override;
};
