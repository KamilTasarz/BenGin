#include "CameraFollow.h"
#include "../Basic/Node.h"
//#include "../Component/Camera.h"
#include "RegisterScript.h"
#include "../Component/CameraGlobals.h"
#include "PlayerController.h"

REGISTER_SCRIPT(CameraFollow);	

void CameraFollow::onAttach(Node* owner)
{
	this->owner = owner;
}

void CameraFollow::onDetach()
{
	owner = nullptr;
}

void CameraFollow::onStart()
{
}

void CameraFollow::onUpdate(float deltaTime)
{
	if (player == nullptr || player->getTagName() != "Player") {
		std::cout << "camera follow - player is nullptr" << std::endl;

		player = owner->scene_graph->root->getChildByTag("Player");
		playerController = player->getComponent<PlayerController>();

		glm::vec3 origin = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3 offset = glm::vec3(0.f, 0.f, offsetZ);
		verticalOffset = 3.f;

		camera->setObjectToFollow(owner, origin);
		camera->setOffsetToFollowingObject(offset);
	}
	else {
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
}

void CameraFollow::onEnd()
{
	// Cleanup if needed
}

void CameraFollow::HandleGravityVirus(float deltaTime) {

}
