#include "CameraFollow.h"
#include "../Basic/Node.h"
//#include "../Component/Camera.h"
#include "RegisterScript.h"
#include "../Component/CameraGlobals.h"

REGISTER_SCRIPT(CameraFollow);	

void CameraFollow::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "CameraFollow::onAttach::" << owner->name << std::endl;
}

void CameraFollow::onDetach()
{
	std::cout << "CameraFollow::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void CameraFollow::onStart()
{
	// Initialize the camera follow state
}

void CameraFollow::onUpdate(float deltaTime)
{
	if (player == nullptr/* || player->getTagName() != "Player"*/) {
		std::cout << "camera follow - player is nullptr" << std::endl;

		player = owner->scene_graph->root->getChildByTag("Player");

		glm::vec3 origin = glm::vec3(0.f, 3.f, 0.f);
		glm::vec3 offset = glm::vec3(0.f, offsetY, offsetZ);
		camera->setObjectToFollow(owner, origin);
		camera->setOffsetToFollowingObject(offset);
	}
	else {
		//std::cout << "following player: " << player->getName() << std::endl;

		glm::vec3 targetPosition = player->transform.getLocalPosition();
		glm::vec3 currentPosition = owner->transform.getLocalPosition();
		glm::vec3 newPosition = glm::mix(currentPosition, targetPosition, smoothing * deltaTime);
		owner->transform.setLocalPosition(newPosition);

		std::cout << "Camera target position: " << targetPosition.x << ", " << targetPosition.y << ", " << targetPosition.z << std::endl;
		std::cout << "Camera current position: " << currentPosition.x << ", " << currentPosition.y << ", " << currentPosition.z << std::endl;
		std::cout << "Camera new position: " << newPosition.x << ", " << newPosition.y << ", " << newPosition.z << std::endl;
	}
}

void CameraFollow::onEnd()
{
	// Cleanup if needed
}
