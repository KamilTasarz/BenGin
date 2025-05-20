#include "Checkpoint.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"

REGISTER_SCRIPT(Checkpoint);

void Checkpoint::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "Checkpoint::onAttach::" << owner->name << std::endl;
}

void Checkpoint::onDetach()
{
	std::cout << "Checkpoint::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Checkpoint::onStart()
{
	std::cout << "Checkpoint::onStart::" << owner->name << std::endl;
}

void Checkpoint::onUpdate(float deltaTime)
{
	std::cout << "Checkpoint::onUpdate::" << owner->name << std::endl;
}

void Checkpoint::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		Node* playerSpawner = owner->scene_graph->root->getChildByTag("PlayerSpawner");
		playerSpawner->transform.setLocalPosition(owner->transform.getLocalPosition() * glm::vec3(1.f, 1.f, 0.f));
	}
}
