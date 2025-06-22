#include "Box.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "../GameManager.h"

REGISTER_SCRIPT(Box);

void Box::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "Box::onAttach::" << owner->name << std::endl;
}

void Box::onDetach()
{
	std::cout << "Box::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Box::onStart()
{
	rb = owner->getComponent<Rigidbody>();

	auto* audio = ServiceLocator::getAudioEngine();
	sfxId = audio->PlayMusic(audio->pushing, /*GameManager::instance().sfxVolume **/ 65.f);
}

void Box::onUpdate(float deltaTime)
{
	glm::vec3 pos = owner->transform.getGlobalPosition();
	std::cout << "Box::onUpdate::" << owner->name << "pozycja: " << pos.x  << ", " << pos.y << ", " << pos.z << std::endl;

	if (sfxId != -1 && rb && abs(rb->velocityX) <= 0.1f) {
		auto* audio = ServiceLocator::getAudioEngine();
		audio->pauseSound(sfxId);
	}
	else if(sfxId != -1 && rb && abs(rb->velocityX) > 0.1f) {
		auto* audio = ServiceLocator::getAudioEngine();
		audio->resumeSound(sfxId);
	}
}
