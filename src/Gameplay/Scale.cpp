#include "Scale.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "GameMath.h"
#include "PlayerController.h"


REGISTER_SCRIPT(Scale);

void Scale::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "Scale::onAttach::" << owner->name << std::endl;

	rb = owner->getComponent<Rigidbody>();

	startPos1 = owner->transform.getLocalPosition();
	if (secondScale != NULL) startPos2 = secondScale->transform.getLocalPosition();
}

void Scale::onDetach()
{
	std::cout << "Scale::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Scale::onStart()
{
	std::cout << "start szalek -----------------------------------------------------------" << std::endl;
	
	rb = owner->getComponent<Rigidbody>();

	startPos1 = owner->transform.getLocalPosition();
	startPos2 = secondScale->transform.getLocalPosition();
	/*returnToPosition = true;
	moveHorizontally = false;
	isPlayerOn = false;*/
}

void Scale::onUpdate(float deltaTime)
{
	if (secondScale == NULL) return;
	else if (startPos2 == glm::vec3(0.f)) startPos2 = secondScale->transform.getLocalPosition();

	if (setStartPos) {
		startPos1 = owner->transform.getLocalPosition();
		startPos2 = secondScale->transform.getLocalPosition();
		setStartPos = false;
	}

	if (!isPlayerOn && !moveHorizontally && returnToPosition) {
		glm::vec3 position1 = owner->transform.getLocalPosition();
		glm::vec3 newPosition = GameMath::MoveTowards(position1, startPos1, 1.f * deltaTime);
		owner->transform.setLocalPosition(newPosition);

		glm::vec3 position2 = secondScale->transform.getLocalPosition();
		glm::vec3 newPosition2 = GameMath::MoveTowards(position2, startPos2, 1.f * deltaTime);
		secondScale->transform.setLocalPosition(newPosition2);
	}
	else {
		if (moveHorizontally) {
			glm::vec3 position1 = owner->transform.getLocalPosition();
			//owner->transform.setLocalPosition(position1 - glm::vec3(0.f, 2.f * deltaTime, 0.f));

			float offset = startPos1.x - position1.x;

			secondScale->transform.setLocalPosition(startPos2 + glm::vec3(offset, 0.f, 0.f));
		}
		else if (isPlayerOn) {
			owner->getComponent<Rigidbody>()->velocityY = 0.f;

			float loweringSpeed = 0.2f;
			if (isPlayerHeavy) loweringSpeed = 2.f;

			glm::vec3 position1 = owner->transform.getLocalPosition();
			owner->transform.setLocalPosition(position1 - glm::vec3(0.f, loweringSpeed * deltaTime, 0.f));

			float offset = startPos1.y - position1.y;

			secondScale->transform.setLocalPosition(startPos2 + glm::vec3(0.f, offset, 0.f));
		}
	}

	if (timer > 0.f) {
		timer -= deltaTime;
		if (timer <= 0.f) {
			timer = 0.f;
			isPlayerOn = false;
			isPlayerHeavy = false;
		}
	}



	// draw line between pins and platforms
	//glm::vec3 leftPinPos = leftPin->transform.getLocalPosition();
	//glm::vec3 rightPinPos = rightPin->transform.getLocalPosition();
	//glm::vec3 leftPlatformPos = owner->transform.getLocalPosition();
	//glm::vec3 rightPlatformPos = secondScale->transform.getLocalPosition();

	//TODO: draw line
}

void Scale::onEnd()
{
}

void Scale::onStayCollision(Node* other)
{
	if (other->getTagName() == "Player") {
		isPlayerOn = true;

		if (other->getComponent<PlayerController>()->virusType == "black") {
			isPlayerHeavy = true;
		}
	}
}

void Scale::onExitCollision(Node* other)
{
	if (other->getTagName() == "Player") {
		timer = 0.1f;
	}
}
