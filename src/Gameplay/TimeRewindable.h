// TimeRewindable.h
#pragma once
#include <deque>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Script.h"

class TimeSnapshot {
public:
    glm::vec3 position;
    glm::quat rotation;
};

class TimeRewindable : public Script {
public:
	using SelfType = TimeRewindable;

    std::deque<TimeSnapshot> history;
    float maxTime = 10.f;         // ile sekund przechowuje
    float rewindSpeed = 2.f;     // x razy szybciej ni¿ normalny czas
    float accumulatedTime = 0.f;  // do zarz¹dzania maxTime

    bool isRewinding = false;

	void onAttach(Node* owner) override;
	void onDetach() override;
    void onUpdate(float deltaTime) override;

    void pushSnapshot(const TimeSnapshot& snapshot);
    bool popSnapshot(TimeSnapshot& outSnapshot); // zwraca false jak pusto
};
