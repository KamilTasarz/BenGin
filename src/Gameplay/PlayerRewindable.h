#pragma once

#include "TimeRewindable.h"
#include "../Basic/Node.h"

class PlayerController;

class PlayerSnapshot : public TimeSnapshot {
public:
    bool isDead;
    bool isGravityFlipped;
    std::string virusType;
    std::string tagName;
};

class PlayerRewindable : public TimeRewindable {
public:
    using SelfType = PlayerRewindable;

    std::deque<PlayerSnapshot> playerHistory;

    bool isDead = false;
    bool isGravityFlipped = false;
	std::string virusType = "none";
    std::string tagName = "Default";
	PlayerController* playerController = nullptr;

	void onStart() override {
		playerController = owner->getComponent<PlayerController>();
	}

    void onUpdate(float deltaTime) override;
};
