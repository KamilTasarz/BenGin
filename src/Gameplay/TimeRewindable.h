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
    float rewindSpeed = 2.f;
    bool isRewinding = false;
	glm::vec3 lastCheckpointPos;

    virtual void onUpdate(float deltaTime) override;
    void resetHistory();

protected:
    virtual std::shared_ptr<ITimeSnapshot> createSnapshot() = 0;
    virtual void applySnapshot(const std::shared_ptr<ITimeSnapshot>& snapshot) = 0;

private:
    void pushSnapshot(std::shared_ptr<ITimeSnapshot> snapshot);
};
