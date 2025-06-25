#include "ControlsPanel.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "GameManager.h"
#include "../ResourceManager.h"

REGISTER_SCRIPT(ControlsPanel);

void ControlsPanel::onAttach(Node* owner)
{
	this->owner = owner;
}

void ControlsPanel::onDetach()
{
	owner = nullptr;
}

void ControlsPanel::onStart()
{
}

void ControlsPanel::onUpdate(float deltaTime)
{
	if (gamepadInput != GameManager::instance().isGamepadConnected) {
		gamepadInput = GameManager::instance().isGamepadConnected;
		modelChanged = false;
	}
	
	if (!modelChanged && !gamepadInput) {
		auto model = ResourceManager::Instance().getModel(keyboardModelID);
		owner->pModel = model;
	}
	else if (!modelChanged && gamepadInput) {
		auto model = ResourceManager::Instance().getModel(ControllerModelID);
		owner->pModel = model;
	}
}
