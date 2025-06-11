#include "Electrified.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "../PlayerController.h"
#include "../GameManager.h"

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
		//owner->scene_graph->root->forceUpdateSelfAndChild();
		
		glm::vec3 pos = owner->transform.getGlobalPosition();
		auto* audio = ServiceLocator::getAudioEngine();
		sfxId = audio->PlayMusic(audio->electricity, GameManager::instance->sfxVolume * 70.f, pos);
		audio->SetChannel3dMinMaxDistance(sfxId, 3.0f, 20.0f);
	}	
}

void Electrified::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		other->addComponent(std::make_unique<Electrified>());
		other->getComponent<PlayerController>()->Die(false, true);

		glm::vec3 pos = owner->transform.getGlobalPosition();

		auto* audio = ServiceLocator::getAudioEngine();
		audio->PlaySFX(audio->electrified, GameManager::instance->sfxVolume * 70.f);
	}
}
