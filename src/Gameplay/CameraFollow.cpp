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
	positionZ = owner->transform.getLocalPosition().z;
}

void CameraFollow::onUpdate(float deltaTime)
{
	if (player == nullptr || player->getTagName() != "Player") {
		std::cout << "camera follow - player is nullptr" << std::endl;

		player = owner->scene_graph->root->getChildByTag("Player");
		//std::cout << "current player: " << player->getName() << ", tag: " << player->getTagName() << std::endl;

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

		//std::cout << "Following: " << player->getName() << ", tag: " << player->getTagName() <<   std::endl;
	}

	glm::vec3 pos = owner->transform.getLocalPosition();
	owner->transform.setLocalPosition(glm::vec3(pos.x, pos.y, positionZ));
}

void CameraFollow::onEnd()
{
	// Cleanup if needed
}
