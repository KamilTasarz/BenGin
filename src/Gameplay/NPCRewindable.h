#pragma once

#include "TimeRewindable.h"
#include <glm/glm.hpp>
#include "Puzzles/TVManager.h"

class NPCController;

class NPCSnapshot : public ITimeSnapshot {
public:
    bool isFleeing = false;
    bool isCatched = false;
    bool isActive = false;
	bool isPhysic = false;
    glm::vec3 targetVelocity{ 0.0f };
    glm::vec3 currentVelocity{ 0.0f };
};

class NPCRewindable : public TimeRewindable {
public:
    TVManager* tv = nullptr;

	void onAttach(Node* owner) override;
	void onDetach() override;
    void onStart() override;

protected:
    std::shared_ptr<ITimeSnapshot> createSnapshot() override;
    void applySnapshot(const std::shared_ptr<ITimeSnapshot>& snapshot) override;
};
