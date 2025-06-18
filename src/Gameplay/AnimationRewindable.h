#pragma once

#include "TimeRewindable.h"
#include "../Basic/Node.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class PlayerAnimationController;
class Animation;

struct AnimationSnapshot : public ITimeSnapshot {
    bool isTurning;
    bool gravityFlipped;
    bool facingRight;
	bool isDead = false; // Whether the player is dead
	bool allFinished = false; // Whether all animations have finished
    glm::quat targetRotation;
    int model_id;
	std::string currentAnimation;
	float time_animation = 0.f; // Current time in the animation
};

class AnimationRewindable : public TimeRewindable {
public:
    PlayerAnimationController* animationController = nullptr;

    void onAttach(Node* owner) override;
    void onDetach() override;
    void onStart() override;

protected:
    std::shared_ptr<ITimeSnapshot> createSnapshot() override;
    void applySnapshot(const std::shared_ptr<ITimeSnapshot>& snapshot) override;
};
