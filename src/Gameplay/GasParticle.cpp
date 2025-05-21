#include "GasParticle.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "GameMath.h"
#include "PlayerController.h"

REGISTER_SCRIPT(GasParticle);

void GasParticle::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "GasParticle::onAttach::" << owner->name << std::endl;
	scale = owner->transform.getLocalScale();
	growTime = 0.f;
	timer = 0.f;
}

void GasParticle::onDetach()
{
	std::cout << "GasParticle::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void GasParticle::onStart()
{
	std::cout << "GasParticle::onStart::" << owner->name << std::endl;
	float randomScale = GameMath::RandomFloat(0.8f, 1.2f);
	scale = glm::vec3(randomScale, randomScale, randomScale);
	growTime = 2.5f;
	timer = 0.f;
}

void GasParticle::onUpdate(float deltaTime)
{
	timer += deltaTime;
	//std::cout << timer << std::endl;

	if (timer < growTime) {
		float scaleFactor = timer / growTime;
		//glm::vec3 newScale = glm::mix(glm::vec3(0.f), scale, scaleFactor);
		owner->transform.setLocalScale(scale * scaleFactor);
	}
	else if (timer >= growTime && timer < 20.f) {
		float scaleDiff = sin(timer) * 0.2f;
		glm::vec3 newScale = glm::mix(owner->transform.getLocalScale(), glm::vec3(scale.x + scaleDiff, scale.y + scaleDiff, scale.z + scaleDiff), 1.f * deltaTime);
		owner->transform.setLocalScale(newScale);
	}
	else if (timer >= 20.f && timer < 23.f) {
		float scaleFactor = scale.x / 3.f * (23.f - timer);
		owner->transform.setLocalScale(scale * scaleFactor);
	}
	else {
		owner->scene_graph->deleteChild(owner);
		//owner->setActive(false);
	}
}

void GasParticle::onStayCollisionLogic(Node * other)
{
	if (other->getTagName() == "Player") {
		other->getComponent<PlayerController>()->Die(true);
	}
}


