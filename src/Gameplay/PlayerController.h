#pragma once

#include "Script.h"

class PlayerController : public Script
{
public:
    PlayerController() = default;
    virtual ~PlayerController() = default;

    void onAttach(Node* owner) override;

    void onDetach() override;

    void onStart() override;

    void onUpdate(float deltaTime) override;

    void onEnd() override;

    void onCollision(Node* other) override;
};

