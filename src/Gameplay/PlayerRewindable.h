#pragma once

#include "TimeRewindable.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>

class PlayerController;

class PlayerSnapshot : public ITimeSnapshot {
public:
    bool isDead;
    bool isGravityFlipped;
	bool isElectrified;
    std::string virusType;
    std::string tagName;
    std::string LayerName;
};

class PlayerRewindable : public TimeRewindable {
public:
    PlayerController* playerController = nullptr;

    void onAttach(Node* owner) override;
	void onDetach() override;
    void onStart() override;
	void onEnd() override;

protected:
    std::shared_ptr<ITimeSnapshot> createSnapshot() override;
    void applySnapshot(const std::shared_ptr<ITimeSnapshot>& snapshot) override;
};
