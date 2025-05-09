#pragma once

#include "../System/Component.h"
#include "../System/ServiceLocator.h"

class Script : public Component
{
public:
    Script() = default;
    virtual ~Script() = default;



    // Zwraca wskaźnik na właściciela (Node)
    Node* getOwner() const { return owner; }


};

