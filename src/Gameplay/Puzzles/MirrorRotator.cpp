#include "MirrorRotator.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"

#include "../../System/Rigidbody.h"
REGISTER_SCRIPT(MirrorRotator);

void MirrorRotator::onAttach(Node* owner)
{
	this->owner = owner;
}

void MirrorRotator::onDetach()
{
	owner = nullptr;
}

void MirrorRotator::onStart()
{
	startPosition = owner->transform.getLocalPosition();
	startRotation = mirrorNode->transform.getLocalRotation();

	owner->getComponent<Rigidbody>()->lockPositionY = true;
}

void MirrorRotator::onUpdate(float deltaTime)
{
	glm::vec3 currentPosition = owner->transform.getLocalPosition();
	float newPositionX = glm::clamp(currentPosition.x, startPosition.x - movingRange, startPosition.x + movingRange);

	if (newPositionX != currentPosition.x) {
		owner->transform.setLocalPosition(glm::vec3(newPositionX, currentPosition.y, currentPosition.z));
	}

	float offset = newPositionX - startPosition.x;
	float angle = rotationAngle * offset / movingRange;	

	glm::quat newRotation = startRotation * glm::quat(glm::vec3(glm::radians(angle), 0.f, 0.f));
	mirrorNode->transform.setLocalRotation(newRotation);
}
