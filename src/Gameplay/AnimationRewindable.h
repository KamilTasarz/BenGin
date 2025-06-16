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
    glm::quat targetRotation;
    Animation* currentAnimation = nullptr;
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
