#include "Fan.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "../../System/Rigidbody.h"
#include "../PlayerController.h"
#include "../GameManager.h"

REGISTER_SCRIPT(Fan);

void Fan::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "Fan::onAttach::" << owner->name << std::endl;
}

void Fan::onDetach()
{
	std::cout << "Fan::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Fan::onStart()
{
	fan = owner->parent->getChildByNamePart("fan");
	particleEmitter = owner->getComponent<Particles>();
}

void Fan::onUpdate(float deltaTime)
{
	wavyPower = verticalPower;
	wavyPower += sin(glfwGetTime() * 0.2);

	// gradually start/stop rotating the fan
	float smoothing = 2.f;
	fanSpeed += (targetFanSpeed - fanSpeed) * smoothing * deltaTime;
		
	if (fanSpeed > 0.01f && fan != owner) {
		glm::quat currentRotation = fan->transform.getLocalRotation();
		float rotationAnlge = fanSpeed * deltaTime;
		currentRotation = glm::rotate(currentRotation, rotationAnlge, glm::vec3(0.f, 0.f, 1.f));
		fan->transform.setLocalRotation(currentRotation);
	}

	auto* audio = ServiceLocator::getAudioEngine();

	// Wentylator NIEaktywny → zatrzymaj dźwięk, jeśli gra
	if (!isActive) {
		if (sfxId != -1 && audio->IsPlaying(sfxId)) {
			audio->pauseSound(sfxId);
			sfxId = -1;
		}
		targetFanSpeed = 0.f;
		if (particleEmitter) particleEmitter->emit = false;

		return;
	}

	// Wentylator aktywny → wznowienie tylko jeśli NIE gra
	if (sfxId == -1) {
		glm::vec3 pos = owner->transform.getGlobalPosition();
		sfxId = audio->PlayMusic(audio->fan, GameManager::instance().sfxVolume * 90.f, pos);
		audio->SetChannel3dMinMaxDistance(sfxId, 3.f, 20.0f);
	}

	targetFanSpeed = 10.f;
	if (particleEmitter) particleEmitter->emit = true;
}

void Fan::onStayCollisionLogic(Node* other)
{
	if (!isActive) {
		return;
	}

	if (other->getTagName() == "Player" || other->getTagName() == "Box") {
		if (other->getComponent<PlayerController>() != nullptr) {
			if (other->getComponent<PlayerController>()->virusType == "black") {
				return;
			}
		}

		Rigidbody* rb = other->getComponent<Rigidbody>();
		if (rb) {
			if (verticalPower != 0.f) {
				rb->overrideVelocityY = true;
				rb->targetVelocityY = wavyPower;
			}
			if (horizontalPower != 0.f) {
				rb->overrideVelocityX = true;
				rb->targetVelocityX = horizontalPower;
			}
		}
	}
}

void Fan::onExitCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player" || other->getTagName() == "Box") {
		Rigidbody* rb = other->getComponent<Rigidbody>();
		if (rb) {
			rb->overrideVelocityY = false;
			rb->overrideVelocityX = false;
			rb->targetVelocityY = 0.f;
			rb->targetVelocityX = 0.f;
		}
	}
}

void Fan::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player" || other->getTagName() == "Box") {
		auto* audio = ServiceLocator::getAudioEngine();
		//sfxId = audio->PlayMusic(audio->wind_blow, 70.f);
	}
}


