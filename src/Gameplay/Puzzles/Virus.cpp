#include "Virus.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "../PlayerController.h"
#include "../../System/Rigidbody.h"
#include "../../Component/CameraGlobals.h"
#include "../CameraFollow.h"
#include "../GameManager.h"
#include "../../ResourceManager.h"

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
}

void Virus::onUpdate(float deltaTime)
{
	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_R) == GLFW_PRESS || player && player->getTagName() == "Box") {
		isCollected = false;
		player == nullptr;

		if (blue) {
			auto model = ResourceManager::Instance().getModel(25);
			owner->pModel = model;
		}
		else if (green) {
			auto model = ResourceManager::Instance().getModel(23);
			owner->pModel = model;
		}
		else if (black) {
			auto model = ResourceManager::Instance().getModel(24);
			owner->pModel = model;
		}
		else {
			std::cout << "Unknown virus type!" << std::endl;
		}
	}
}

void Virus::onEnd()
{
}

void Virus::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		//std::cout << "Ser podniesiony - " << owner->name << std::endl;

		if (isCollected) return;

		player = other;

		auto* audio = ServiceLocator::getAudioEngine();
		audio->PlaySFX(audio->eating, GameManager::instance->sfxVolume * 80.f);

		isCollected = true;

		ApplyEffect(other);
	}
}

void Virus::ApplyEffect(Node* target)
{
	PlayerController* player = target->getComponent<PlayerController>();

	target->getComponent<Rigidbody>()->gravity = -32.f;
	target->getComponent<Rigidbody>()->mass = 1.f;
	player->speed = 9.f;
	player->isGravityFlipped = false;
	player->jumpForce = 19.f;
	player->virusType = "none";
	camera->object_to_follow->getComponent<CameraFollow>()->verticalOffset = 3.f;

	VirusEffect(target);

	std::cout << "Virus effect applied to: " << target->name << std::endl;
}

void Virus::VirusEffect(Node* target)
{
	PlayerController* player = target->getComponent<PlayerController>();

	if (blue) {
		target->changeColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		player->isGravityFlipped = false;
		target->getComponent<Rigidbody>()->gravity = -32.f;
		target->getComponent<Rigidbody>()->mass = 0.4f;
		player->jumpForce *= 1.2f;
		player->virusType = "blue";

		auto model = ResourceManager::Instance().getModel(60);
		owner->pModel = model;
	}
	else if (green) {
		target->changeColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

		camera->object_to_follow->getComponent<CameraFollow>()->verticalOffset = -1.f;
		player->isGravityFlipped = true;
		target->getComponent<Rigidbody>()->gravity = 32.f;
		player->virusType = "green";

		auto model = ResourceManager::Instance().getModel(62);
		owner->pModel = model;
	}
	else if (black) {
		target->changeColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

		target->getComponent<Rigidbody>()->mass = 25.f;
		player->speed *= 0.7f;
		player->jumpForce *= 0.8f;
		player->virusType = "black";

		auto model = ResourceManager::Instance().getModel(61);
		owner->pModel = model;
	}
	else {
		std::cout << "Unknown virus type!" << std::endl;
	}
}


