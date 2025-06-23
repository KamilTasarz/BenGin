#pragma once

#include <deque>
#include <memory>
#include "Script.h"

class ITimeSnapshot {
public:
    virtual ~ITimeSnapshot() = default;
};

class TimeRewindable : public Script {
public:
    std::deque<std::shared_ptr<ITimeSnapshot>> history;
    float maxTime = 10.f;
	float rewindTime = 0.f;
    int rewindSpeed = 2;
    bool isRewinding = false;
    bool hasReleasedRewindKey = true;
	glm::vec3 lastCheckpointPos;
    int sfxId = -1;

    bool isPadButtonPressed(int button);
    bool isPadButtonReleased(int button);

    virtual void onUpdate(float deltaTime) override;
    void resetHistory();

protected:
    virtual std::shared_ptr<ITimeSnapshot> createSnapshot() = 0;
    virtual void applySnapshot(const std::shared_ptr<ITimeSnapshot>& snapshot) = 0;

private:
    void pushSnapshot(std::shared_ptr<ITimeSnapshot> snapshot);
};
