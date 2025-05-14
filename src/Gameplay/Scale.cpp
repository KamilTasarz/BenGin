#include "Scale.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "GameMath.h"


REGISTER_SCRIPT(Scale);

void Scale::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "Scale::onAttach::" << owner->name << std::endl;

	rb = owner->getComponent<Rigidbody>();

	startPos1 = owner->transform.getLocalPosition();
	startPos2 = secondScale->transform.getLocalPosition();
}

void Scale::onDetach()
{
	std::cout << "Scale::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Scale::onStart()
{
	rb = owner->getComponent<Rigidbody>();

	startPos1 = owner->transform.getLocalPosition();
	startPos2 = secondScale->transform.getLocalPosition();
}

void Scale::onUpdate(float deltaTime)
{
	if (!isPlayerOn) {
		glm::vec3 position1 = owner->transform.getLocalPosition();
		glm::vec3 newPosition = GameMath::MoveTowards(position1, startPos1, 1.f * deltaTime);
		owner->transform.setLocalPosition(newPosition);

		glm::vec3 position2 = secondScale->transform.getLocalPosition();
		glm::vec3 newPosition2 = GameMath::MoveTowards(position2, startPos2, 1.f * deltaTime);
		secondScale->transform.setLocalPosition(newPosition2);
	}
	else {
		owner->getComponent<Rigidbody>()->velocityY = 0.f;

		glm::vec3 position1 = owner->transform.getLocalPosition();
		owner->transform.setLocalPosition(position1 - glm::vec3(0.f, 2.f * deltaTime, 0.f));

		float offset = startPos1.y - position1.y;

		secondScale->transform.setLocalPosition(startPos2 + glm::vec3(0.f, offset, 0.f));
	}

	if (timer > 0.f) {
		timer -= deltaTime;
		if (timer <= 0.f) {
			timer = 0.f;
			isPlayerOn = false;
		}
	}
}

void Scale::onEnd()
{
}

void Scale::onStayCollision(Node* other)
{
	if (other->getTagName() == "Player") {
		isPlayerOn = true;
	}
}

void Scale::onExitCollision(Node* other)
{
	if (other->getTagName() == "Player") {
		timer = 0.2f;
	}
}
