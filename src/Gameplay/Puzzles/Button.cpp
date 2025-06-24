#include "Button.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "../../Component/BoundingBox.h"
#include "Door.h"
#include "Fan.h"
#include "../GameManager.h"
#include "../../ResourceManager.h"

REGISTER_SCRIPT(Button);

void Button::onAttach(Node* owner)
{
	this->owner = owner;
	//std::cout << "Button::onAttach::" << owner->name << std::endl;
}

void Button::onDetach()
{
	//std::cout << "Button::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Button::onStart()
{
	// Initialize the button state
	button = owner;
	body = owner->parent->getChildByNamePart("body");

	if (button == nullptr) {
		button = owner;
	}

	isPressed = false;
	pressingObjects = 0;
	startPos = owner->transform.getLocalPosition();
}

void Button::onUpdate(float deltaTime)
{
	if (button == nullptr) return;
	
	glm::vec3 currentPos = button->transform.getLocalPosition();
	glm::vec3 direction = glm::normalize(targetPos - currentPos);
	float distance = glm::distance(currentPos, targetPos);

	if (distance > 0.02f) {
		float speed = 2.f * 1.f / owner->transform.getLocalScale().y;
		float step = speed * deltaTime;

		if (step >= distance) {
			button->transform.setLocalPosition(targetPos);
		}
		else {
			glm::vec3 newPos = currentPos + direction * step;
			button->transform.setLocalPosition(newPos);
		}
	}
}

void Button::ChangeState(bool state, Node* object)
{
	if (object->getComponent<Door>() != nullptr) {
		object->getComponent<Door>()->ChangeState(state);
	}
	else if (object->getComponent<Fan>() != nullptr) {
		object->getComponent<Fan>()->isActive = state;
	}
	else if (object->getChildByName("gate_up") != nullptr) {
		object->getChildByName("gate_up")->getComponent<Door>()->ChangeState(state);
	}
}

void Button::onCollisionLogic(Node* other) {
	if (other->getTagName() == "Player" || other->getTagName() == "Box") {
		if (pressingObjects == 0) {
			auto* audio = ServiceLocator::getAudioEngine();
			audio->PlaySFX(audio->button_down, GameManager::instance().sfxVolume * 70.f);

			targetPos = startPos - glm::vec3(0.f, 0.15f, 0.f) / owner->transform.getLocalScale().y;

			auto model = ResourceManager::Instance().getModel(42);
			body->pModel = model;

			ChangeState(activate, object);
			if (secondObject) ChangeState(activateSecond, secondObject);
		}

		pressingObjects++;
		isPressed = true;
	}
}

void Button::onStayCollisionLogic(Node* other)
{
	//if (other->getTagName() == "Player" || other->getTagName() == "Box") {
	//	//std::cout << "Button pressed - " << owner->name << std::endl;
	//	isPressed = true;
	//	glm::vec3 newScale = originalSize * glm::vec3(1.f, 0.3f, 1.f);
	//	owner->transform.setLocalScale(newScale);

	//	ChangeState(activate);
	//}
}

void Button::onExitCollisionLogic(Node* other) {
	if (other->getTagName() == "Player" || other->getTagName() == "Box") {
		pressingObjects = std::max(0, pressingObjects - 1);

		if (pressingObjects == 0) {
			isPressed = false;

			targetPos = startPos;
			auto model = ResourceManager::Instance().getModel(43);
			body->pModel = model;

			auto* audio = ServiceLocator::getAudioEngine();
			audio->PlaySFX(audio->button_up, 70.f);

			ChangeState(!activate, object);
			if (secondObject) ChangeState(!activateSecond, secondObject);
		}
	}
}
