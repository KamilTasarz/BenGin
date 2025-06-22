#include "CameraFollow.h"
#include "../Basic/Node.h"
//#include "../Component/Camera.h"
#include "RegisterScript.h"
#include "../Component/CameraGlobals.h"
#include "PlayerController.h"
#include "GameManager.h"

REGISTER_SCRIPT(CameraFollow);	

CameraFollow* CameraFollow::instance = nullptr;

void CameraFollow::onAttach(Node* owner)
{
	CameraFollow::instance = this;
	this->owner = owner;
}

void CameraFollow::onDetach()
{
	owner = nullptr;
}

void CameraFollow::onStart()
{
	glm::vec3 origin = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 offset = glm::vec3(0.f, 0.f, offsetZ);
	verticalOffset = 3.f;

	camera->setObjectToFollow(owner, origin);
	camera->setOffsetToFollowingObject(offset);
}

void CameraFollow::onUpdate(float deltaTime)
{
	player = GameManager::instance().currentPlayer;
	if (player == nullptr) return;

	PlayerController* playerController = player->getComponent<PlayerController>();
	if (playerController == nullptr) return;

	glm::vec3 targetPosition = player->transform.getLocalPosition();

	targetPosition.y += verticalOffset;
	if (playerController->isDying || playerController->isInGas) targetPosition.z -= 10.f;

	glm::vec3 currentPosition = owner->transform.getLocalPosition();
	glm::vec3 newPosition;
	newPosition.x = glm::mix(currentPosition.x, targetPosition.x, smoothing * deltaTime);
	newPosition.y = glm::mix(currentPosition.y, targetPosition.y, smoothing * deltaTime);
	newPosition.z = glm::mix(currentPosition.z, targetPosition.z, smoothing * .2f * deltaTime);

	owner->transform.setLocalPosition(newPosition);
}

void CameraFollow::onEnd()
{
	// Cleanup if needed
}

void CameraFollow::HandleGravityVirus(float deltaTime) {

}
