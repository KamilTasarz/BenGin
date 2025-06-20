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
#include "../GameMath.h"
#include "../../System/Tag.h"

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

	particleEmitter = owner->getComponent<Particles>();
}

void Virus::onUpdate(float deltaTime)
{
	//rewindable->isCollected = isCollected;

	//rewindable->player = player;
	if (!GameManager::instance->currentPlayer) return;
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

	//if (!particles.empty()) {
	//	particleTimer -= deltaTime;
	//	if (particleTimer <= 0.f) {
	//		for (Node* particle : particles) {
	//			//Node* particle = particles.back();
	//			owner->scene_graph->deleteChild(particle);
	//		}
	//		particles.clear();
	//	}
	//	else {
	//		for (Node* particle : particles) {
	//			float newScale = particle->transform.getLocalScale().x - deltaTime * 0.5f;
	//			newScale = clamp(newScale, 0.f, 0.5f);
	//			particle->transform.setLocalScale(glm::vec3(newScale, newScale, newScale));
	//		}
	//	}
	//}
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
	//particleTimer = 0.5f;

	if (particleEmitter) particleEmitter->SpawnBurst(15);

	if (blue) {
		player->virusType = "blue";
		//ShowParticles("Blue_cheese_parts", "feta_parts");
	}
	else if (green) {
		player->virusType = "green";
		//ShowParticles("Green_cheese_parts", "gouda_parts");
	}
	else if (black) {
		player->virusType = "black";
		//ShowParticles("Black_cheese_parts", "havarti_parts");
	}
}

void Virus::ShowParticles(std::string prefabName, std::string particleName) {
	for (int i = 0; i < 12; i++) {
		PrefabInstance* pref = new PrefabInstance(PrefabRegistry::FindPuzzleByName(prefabName), owner->scene_graph, "_" + std::to_string(i), owner->getTransform().getLocalPosition());
		Node* particle = pref->prefab_root->getChildById(0);
		particle->transform.setLocalPosition(owner->transform.getGlobalPosition());
		owner->scene_graph->addChild(particle);

		particles.push_back(particle);

		auto* rb = particle->getComponent<Rigidbody>();
		if (rb) {
			rb->drag = -1.f;
			rb->velocityX = GameMath::RandomFloat(-30.f, 30.f);
			rb->velocityY = GameMath::RandomFloat(-5.f, 5.f);
			rb->velocityZ = GameMath::RandomFloat(-30.f, 30.f);
			rb->useVelocityZ = true;
			particle->AABB->addIgnoredLayer(TagLayerManager::Instance().getLayer("Player"));

			// randomize particle rotation
			glm::vec3 randomRotation = glm::vec3(GameMath::RandomFloat(0.f, 360.f), GameMath::RandomFloat(0.f, 360.f), GameMath::RandomFloat(0.f, 360.f));
			particle->transform.setLocalRotation(glm::quat(glm::vec3(glm::radians(randomRotation.x), glm::radians(randomRotation.y), glm::radians(randomRotation.z))));

			// set particle scale
			float scaleFactor = GameMath::RandomFloat(0.1f, 0.3f);
			particle->transform.setLocalScale(glm::vec3(scaleFactor, scaleFactor, scaleFactor));
		}
	}
}
