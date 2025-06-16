#pragma once

#include "TimeRewindable.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Rigidbody;

struct RigidbodySnapshot : public ITimeSnapshot {
    glm::vec3 position;
    glm::quat rotation;
    bool is_static;
};

class RigidbodyRewindable : public TimeRewindable {
public:
    Rigidbody* rigidbody = nullptr;

	void onAttach(Node* owner) override;
	void onDetach() override;
    void onStart() override;

protected:
    std::shared_ptr<ITimeSnapshot> createSnapshot() override;
    void applySnapshot(const std::shared_ptr<ITimeSnapshot>& snapshot) override;
};
