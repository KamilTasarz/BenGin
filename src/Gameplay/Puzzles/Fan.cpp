#include "Fan.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "../../System/Rigidbody.h"
#include "../PlayerController.h"

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
	glm::vec3 pos = owner->transform.getGlobalPosition();
	auto* audio = ServiceLocator::getAudioEngine();
	//sfxId = audio->PlayMusic(audio->fan, 90.f, pos);
}

void Fan::onUpdate(float deltaTime)
{
	wavyPower = verticalPower;

	wavyPower += sin(glfwGetTime() * 0.2);

	if (!isActive) {
		if (sfxId != -1) {
			auto* audio = ServiceLocator::getAudioEngine();
			//audio->pauseSound(sfxId);
		}

		return;
	}

	auto* audio = ServiceLocator::getAudioEngine();
	//audio->resumeSound(sfxId);
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


