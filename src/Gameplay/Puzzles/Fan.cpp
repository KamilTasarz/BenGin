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
	//glm::vec3 pos = owner->transform.getGlobalPosition();
	//auto* audio = ServiceLocator::getAudioEngine();
	//sfxId = audio->PlayMusic(audio->fan, /*GameManager::instance->sfxVolume **/ 100.f, pos);
	//audio->SetChannel3dMinMaxDistance(sfxId, 3.f, 20.0f);
}

void Fan::onUpdate(float deltaTime)
{
	wavyPower = verticalPower;
	wavyPower += sin(glfwGetTime() * 0.2);

	auto* audio = ServiceLocator::getAudioEngine();

	// Wentylator NIEaktywny → zatrzymaj dźwięk, jeśli gra
	if (!isActive) {
		if (sfxId != -1 && audio->IsPlaying(sfxId)) {
			audio->pauseSound(sfxId);
			sfxId = -1;
		}
		return;
	}

	// Wentylator aktywny → wznowienie tylko jeśli NIE gra
	if (sfxId == -1) {
		glm::vec3 pos = owner->transform.getGlobalPosition();
		sfxId = audio->PlayMusic(audio->fan, GameManager::instance->sfxVolume * 100.f, pos);
		audio->SetChannel3dMinMaxDistance(sfxId, 3.f, 20.0f);
	}
}

void Fan::onStayCollisionLogic(Node* other)
{
	if (!isActive) {
		return;
	}

	if (other->getTagName() == "Player" || other->getTagName() == "Box") {
		if (other->getTagName() == "Player") {
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

void Fan::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player" || other->getTagName() == "Box") {
		auto* audio = ServiceLocator::getAudioEngine();
		//sfxId = audio->PlayMusic(audio->wind_blow, 70.f);
	}
}


