#include "Virus.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "../PlayerController.h"
#include "../../System/Rigidbody.h"
#include "../../Component/CameraGlobals.h"
#include "../CameraFollow.h"
#include "../GameManager.h"
#include "../../ResourceManager.h"
#include "../VirusRewindable.h"

REGISTER_SCRIPT(Virus);

void Virus::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "Virus::onAttach::" << owner->name << std::endl;
}

void Virus::onDetach()
{
	std::cout << "Virus::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Virus::onStart()
{
	rewindable = owner->getComponent<VirusRewindable>();
	if (!rewindable) {
		owner->addComponent(std::make_unique<VirusRewindable>());
		rewindable = owner->getComponent<VirusRewindable>();
	}
}

void Virus::onUpdate(float deltaTime)
{
	//rewindable->isCollected = isCollected;

	//rewindable->player = player;
	PlayerController* player = GameManager::instance->currentPlayer->getComponent<PlayerController>();

	if (!isCollected && !modelChanged || player->isDead) {
		int modelId = -1;
		if (blue) modelId = 25;
		else if (green) modelId = 23;
		else if (black) modelId = 24;

		if (modelId != -1) {
			owner->pModel = ResourceManager::Instance().getModel(modelId);
		}

		isCollected = false;
		modelChanged = true;
	}

	if (isCollected && !modelChanged) {
		int modelId = -1;
		if (blue) modelId = 60;
		else if (green) modelId = 62;
		else if (black) modelId = 61;

		if (modelId != -1) {
			owner->pModel = ResourceManager::Instance().getModel(modelId);
		}

		modelChanged = true;
	}
}

void Virus::onEnd()
{
}

void Virus::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {

		if (isCollected) return;

		auto* audio = ServiceLocator::getAudioEngine();
		audio->PlaySFX(audio->eating, GameManager::instance->sfxVolume * 80.f);

		isCollected = true;
		modelChanged = false;

		VirusEffect(other);
	}
}

void Virus::VirusEffect(Node* target)
{
	PlayerController* player = target->getComponent<PlayerController>();

	if (blue) {
		player->virusType = "blue";
	}
	else if (green) {
		player->virusType = "green";
	}
	else if (black) {
		player->virusType = "black";
	}
}


