#include "Button.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "../../Component/BoundingBox.h"
#include "Door.h"
#include "Fan.h"

REGISTER_SCRIPT(Button);

void Button::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "Button::onAttach::" << owner->name << std::endl;

	originalSize = owner->transform.getLocalScale();
}

void Button::onDetach()
{
	std::cout << "Button::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Button::onStart()
{
	// Initialize the button state
	isPressed = false;
	pressingObjects = 0;
	originalSize = owner->transform.getLocalScale();
}

void Button::onUpdate(float deltaTime)
{
	if (isPressed) {
		//std::cout << "Button pressed" << std::endl;
	}
	else {
		//std::cout << "Button not pressed" << std::endl;
	}
}

void Button::ChangeState(bool state)
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
			audio->PlaySFX(audio->button_down, 80.f);

			glm::vec3 newScale = originalSize * glm::vec3(1.f, 0.3f, 1.f);
			owner->transform.setLocalScale(newScale);

			owner->AABB_logic->min_point_local.y *= 1.f / 0.3f;
			owner->AABB_logic->max_point_local.y *= 1.f / 0.3f;

			ChangeState(activate);
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
			owner->transform.setLocalScale(originalSize);

			owner->AABB_logic->min_point_local.y /= 1.f / 0.3f;
			owner->AABB_logic->max_point_local.y /= 1.f / 0.3f;

			auto* audio = ServiceLocator::getAudioEngine();
			audio->PlaySFX(audio->button_up, 80.f);

			ChangeState(!activate);
		}
	}
}
