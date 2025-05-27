#pragma once

//#include "Script.h"
#include "../../Basic/Node.h"

class IPlayerAnimState {
public:
    virtual ~IPlayerAnimState() = default;

    virtual void enter(Node* owner) = 0;
    virtual void update(Node* owner, float deltaTime) = 0;
    virtual void exit(Node* owner) = 0;
};
