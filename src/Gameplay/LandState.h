#pragma once

#include "IPlayerAnimState.h"

class LandState : public IPlayerAnimState {
public:
    void enter(Node* owner) override;
    void update(Node* owner, float deltaTime) override;
    void exit(Node* owner) override;
};
