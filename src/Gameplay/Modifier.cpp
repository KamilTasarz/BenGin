#include "Modifier.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "GameManager.h"

REGISTER_SCRIPT(Modifier);

void Modifier::onAttach(Node* owner)
{
	this->owner = owner;
}

void Modifier::onDetach()
{
	owner = nullptr;
}

void Modifier::onStart()
{
	int randomIndex = rand() % static_cast<int>(ModifierType::COUNT);
	currentModifier = static_cast<ModifierType>(randomIndex);
}

void Modifier::onCollisionLogic(Node* other)
{
    switch (currentModifier) {
    case ModifierType::Slippery:
        GameManager::instance().globalSmoothing = 1.f;
        break;
    }
}
