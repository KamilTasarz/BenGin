#include "Box.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"

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
	sfxId = audio->PlayMusic(audio->pushing, 65.f);

	//audio->pauseSound(sfxId);
}

void Box::onUpdate(float deltaTime)
{
	if (sfxId != -1 && rb && abs(rb->velocityX) <= 0.1f) {
		auto* audio = ServiceLocator::getAudioEngine();
		audio->pauseSound(sfxId);
	}
	else if(sfxId != -1 && rb && abs(rb->velocityX) > 0.1f) {
		auto* audio = ServiceLocator::getAudioEngine();
		audio->resumeSound(sfxId);
	}
}
