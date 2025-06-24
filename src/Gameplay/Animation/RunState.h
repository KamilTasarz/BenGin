#pragma once

#include "IPlayerAnimState.h"

class RunState : public IPlayerAnimState {
public:
	int sfxId = -1;

    void enter(Node* owner) override;
    void update(Node* owner, float deltaTime) override;
    void exit(Node* owner) override;
    std::string getName() const;
};
