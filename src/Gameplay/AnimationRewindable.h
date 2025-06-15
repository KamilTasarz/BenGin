#pragma once

#include "TimeRewindable.h"
#include "../Basic/Node.h"

class PlayerAnimationController;
class Animation;

class AnimationSnapshot : public TimeSnapshot {
public:
    bool isTurning;
    bool gravityFlipped;
    bool facingRight;
    glm::quat targetRotation;
    Animation* currentAnimation;
};

class AnimationRewindable : public TimeRewindable {
public:
    using SelfType = AnimationRewindable;

    std::deque<AnimationSnapshot> playerHistory;

	bool isTurning = false;
	bool gravityFlipped = false;
	bool facingRight = true;
	glm::quat targetRotation;
    Animation* currentAnimation = nullptr;
    PlayerAnimationController* animationController = nullptr;

    void onStart() override {
        animationController = owner->getComponent<PlayerAnimationController>();
    }

    void onUpdate(float deltaTime) override;
};
