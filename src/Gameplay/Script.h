#pragma once

#include "../System/Component.h"
#include "../System/ServiceLocator.h"
#include "../System/PrefabRegistry.h"

struct Variable
{
	std::string name;
	std::string type;
    size_t offset;
};

#define VARIABLE(type, name) \
    type name; \
    static Variable getField_##name() { \
        return { #name, #type, offsetof(SelfType, name) }; \
    }



class Script : public Component
{
public:
    Script() = default;
    virtual ~Script() = default;



    // Zwraca wskaźnik na właściciela (Node)
    Node* getOwner() const { return owner; }

    virtual std::vector<Variable*> getFields() const { return {}; }
};

