#include "Door.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "../GameManager.h"

REGISTER_SCRIPT(Door);

void Door::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "Door::onAttach::" << owner->name << std::endl;
	//owner->setActive(!isOpen);
}

void Door::onDetach()
{
	std::cout << "Door::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Door::onStart()
{
	std::cout << "Door::onStart::" << owner->name << std::endl;
	door1 = owner->getChildByNamePart("gate_up");
	door2 = owner->getChildByNamePart("gate_down");

	if (!door1 || !door2) return;

	startPos = targetPos = door1->transform.getLocalPosition();
	startPos2 = targetPos2 = door2->transform.getLocalPosition();

	if (activateAlarm) {
		float minDistance = 100.f;
		glm::vec3 ownerPos = owner->transform.getGlobalPosition();

		for (PointLight* point : owner->scene_graph->point_lights) {
			//if (point->parent == owner->parent) light = point;

			glm::vec3 lightPos = point->transform.getGlobalPosition();
			float distance = glm::distance(ownerPos, lightPos);

			if (distance < minDistance) {
				light = point;
				minDistance = distance;
			}
		}
	}

	if (isOpen) {
		if (!openToSide) targetPos = startPos + glm::vec3(0.f, 3.5f, 0.f) / owner->transform.getLocalScale().y;
		else {
			targetPos = startPos + glm::vec3(0.f, 3.f, 0.f) / owner->transform.getLocalScale().y;
			targetPos2 = startPos2 + glm::vec3(0.f, -3.f, 0.f) / owner->transform.getLocalScale().y;
		}
	}
	else {
		if (light) light->is_shining = false;
	}
}

void Door::onUpdate(float deltaTime)
{
	if (door1 == nullptr || door2 == nullptr) {
		return;
	}

	if (!openToSide) {
		glm::vec3 currentPos = door1->transform.getLocalPosition();
		glm::vec3 direction = glm::normalize(targetPos - currentPos);
		float distance = glm::distance(currentPos, targetPos);

		if (distance > 0.02f) {
			float speed = 16.f / owner->transform.getLocalScale().y;
			float step = speed * deltaTime;

			if (step >= distance) {
				door1->transform.setLocalPosition(targetPos);
			}
			else {
				glm::vec3 newPos = currentPos + direction * step;
				door1->transform.setLocalPosition(newPos);
			}
		}
	}
	else {
		glm::vec3 currentPos1 = door1->transform.getLocalPosition();
		glm::vec3 currentPos2 = door2->transform.getLocalPosition();
		glm::vec3 direction1 = glm::normalize(targetPos - currentPos1);
		glm::vec3 direction2 = glm::normalize(targetPos2 - currentPos2);

		float distance1 = glm::distance(currentPos1, targetPos);
		float distance2 = glm::distance(currentPos2, targetPos2);

		if (distance1 > 0.02f) {
			float speed = 10.f * 1.f / owner->transform.getLocalScale().y;
			float step = speed * deltaTime;
			if (step >= distance1) {
				door1->transform.setLocalPosition(targetPos);
				door2->transform.setLocalPosition(targetPos2);
			}
			else {
				glm::vec3 newPos = currentPos1 + direction1 * step;
				door1->transform.setLocalPosition(newPos);

				newPos = currentPos2 + direction2 * step;
				door2->transform.setLocalPosition(newPos);
			}
		}
	}
}

void Door::ChangeState(bool state)
{
	if (overrideState) return;

	if (state) {
		targetPos = startPos;
		targetPos2 = startPos2;
		isOpen = false;

		if (light) {
			light->is_shining = false;
			light->is_alarm = false;
		}
	}
	else {
		if (!openToSide) targetPos = startPos + glm::vec3(0.f, 3.5f, 0.f) / owner->transform.getLocalScale().y;
		else {
			targetPos = startPos + glm::vec3(0.f, 3.f, 0.f) / owner->transform.getLocalScale().y;
			targetPos2 = startPos2 + glm::vec3(0.f, -3.f, 0.f) / owner->transform.getLocalScale().y;
		}
		isOpen = true;

		if (light) {
			light->is_shining = true;
			light->is_alarm = true;
		}
	}

	if (sfxId != -1) {
		auto* audio = ServiceLocator::getAudioEngine();
		audio->stopSound(sfxId);
	}

	glm::vec3 pos = owner->transform.getGlobalPosition();
	auto* audio = ServiceLocator::getAudioEngine();
	sfxId = audio->PlayMusic(audio->gate_open, GameManager::instance().sfxVolume * 80.f, pos);
	audio->SetChannel3dMinMaxDistance(sfxId, 3.0f, 20.0f);
}

void Door::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		ChangeState(true);
		overrideState = true;
	}
}