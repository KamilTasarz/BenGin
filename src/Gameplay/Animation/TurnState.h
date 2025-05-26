#pragma once

#include "IPlayerAnimState.h"
#include "../../Basic/Node.h"

class TurnState : public IPlayerAnimState {
public:
	void enter(Node* owner) override;
	void update(Node* owner, float deltaTime) override;
	void exit(Node* owner) override;
};
