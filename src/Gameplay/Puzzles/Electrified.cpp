#include "Electrified.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "../PlayerController.h"

REGISTER_SCRIPT(Electrified);

void Electrified::onAttach(Node* owner)
{
	this->owner = owner;
	this->owner->is_logic_active = true;
	std::cout << "Electrified::onAttach::" << owner->name << std::endl;
}

void Electrified::onDetach()
{
	std::cout << "Electrified::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Electrified::onStart()
{
	if (owner->getTagName() != "Player") {
		glm::vec3 pos = owner->transform.getGlobalPosition();
		auto* audio = ServiceLocator::getAudioEngine();
		audio->PlayMusic(audio->electricity, 100.f, pos);
	}
}

void Electrified::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		other->addComponent(std::make_unique<Electrified>());
		other->getComponent<PlayerController>()->Die(false, true);

		auto* audio = ServiceLocator::getAudioEngine();
		audio->PlaySFX(audio->electrified, 70.f);
	}
}
