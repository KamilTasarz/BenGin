#include "Door.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "../GameManager.h"

REGISTER_SCRIPT(Door);

void Door::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "Door::onAttach::" << owner->name << std::endl;
	//owner->setActive(!isOpen);
}

void Door::onDetach()
{
	std::cout << "Door::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Door::onStart()
{
	std::cout << "Door::onStart::" << owner->name << std::endl;
	startPos = targetPos = owner->transform.getLocalPosition();

	if (isOpen) {
		if (!openToSide) targetPos = startPos + glm::vec3(0.f, 3.5f, 0.f) * 1.f / owner->parent->transform.getLocalScale().y;
		else targetPos = startPos + glm::vec3(3.5f, 0.f, 0.f) * 1.f / owner->parent->transform.getLocalScale().y;
	}
}

void Door::onUpdate(float deltaTime)
{
	glm::vec3 currentPos = owner->transform.getLocalPosition();
	glm::vec3 direction = glm::normalize(targetPos - currentPos);
	float distance = glm::distance(currentPos, targetPos);

	if (distance > 0.02f) {
		float speed = 16.f * 1.f / owner->parent->transform.getLocalScale().y;
		float step = speed * deltaTime;

		if (step >= distance) {
			owner->transform.setLocalPosition(targetPos);
		}
		else {
			glm::vec3 newPos = currentPos + direction * step;
			owner->transform.setLocalPosition(newPos);
		}
	}

}

void Door::ChangeState(bool state)
{
	if (overrideState) return;
	
	if (state) {
		targetPos = startPos;
		isOpen = false;
	}
	else {
		if (!openToSide) targetPos = startPos + glm::vec3(0.f, 3.5f, 0.f) * 1.f / owner->parent->transform.getLocalScale().y;
		else targetPos = startPos + glm::vec3(3.5f, 0.f, 0.f) * 1.f / owner->parent->transform.getLocalScale().y;
		isOpen = true;
	}

	if (sfxId != -1) {
		auto* audio = ServiceLocator::getAudioEngine();
		audio->stopSound(sfxId);
	}

	glm::vec3 pos = owner->transform.getLocalPosition();
	auto* audio = ServiceLocator::getAudioEngine();
	sfxId = audio->PlayMusic(audio->gate_open, GameManager::instance->sfxVolume * 90.f, pos);
	audio->SetChannel3dMinMaxDistance(sfxId, 4.0f, 20.0f);
}

void Door::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		ChangeState(true);
		overrideState = true;
	}
}




