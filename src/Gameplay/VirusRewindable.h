#pragma once
#include "TimeRewindable.h"
#include "../Basic/Node.h"

class Virus;

class VirusSnapshot : public TimeSnapshot {
public:
	bool isCollected;
	Node* player;
};

class VirusRewindable : public TimeRewindable {
public:
    using SelfType = VirusRewindable;

    std::deque<VirusSnapshot> virusHistory;

    bool isCollected = false;
    Node* player = nullptr;
	Virus* virus = nullptr;

	void onStart() override {
		virus = owner->getComponent<Virus>();
	}
    void onUpdate(float deltaTime) override;
};



