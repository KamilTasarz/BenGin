#pragma once

#include "Script.h"

class PlayerController : public Script
{
public:
    PlayerController() = default;
    virtual ~PlayerController() = default;

    virtual void onAttach(Node* owner) override;

    virtual void onDetach() override;

    void onStart() override;

    void onUpdate(float deltaTime) override;

    void onEnd() override;
};

