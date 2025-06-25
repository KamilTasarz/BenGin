#include "Platform.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "../System/Rigidbody.h"
#include "../Component/BoundingBox.h"
#include "../System/Tag.h"
#include "GameManager.h"
#include "PlayerController.h"

REGISTER_SCRIPT(Platform);

bool Platform::isPadButtonPressed(int button) {

	if (!glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) return false;
	GLFWgamepadstate state;
	if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
		return state.buttons[button] == GLFW_PRESS;
	}
	return false;

}

float Platform::getPadAxis(int axis) {

	if (!glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) return 0.0f;
	GLFWgamepadstate state;
	if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
		return state.axes[axis];
	}
	return 0.0f;

}

void Platform::onAttach(Node* owner)
{
	this->owner = owner;
	owner->AABB->addIgnoredLayer(TagLayerManager::Instance().getLayer("NPC"));

}

void Platform::onDetach()
{
	//std::cout << "Platform::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void Platform::onStart() {
	startMinPos = owner->AABB_logic->min_point_local;
	startMaxPos = owner->AABB_logic->max_point_local;
}

void Platform::onUpdate(float deltaTime)
{

	const float axisY = getPadAxis(GLFW_GAMEPAD_AXIS_LEFT_Y);

	bool isFlippedAndPressedUp = flipped && (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_UP) == GLFW_PRESS || isPadButtonPressed(GLFW_GAMEPAD_BUTTON_DPAD_UP) || axisY < -0.1f);

	downPressed = (
		glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_S) == GLFW_PRESS ||
		glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_DOWN) == GLFW_PRESS ||
		isPadButtonPressed(GLFW_GAMEPAD_BUTTON_DPAD_DOWN) ||
		axisY > 0.1f
		);

	if (downPressed || isFlippedAndPressedUp) {
		owner->AABB->addIgnoredLayer(TagLayerManager::Instance().getLayer("Player"));
		timer = 0.3f;
	}
	
	if (timer > 0.f) {
		timer -= deltaTime;
		if (timer <= 0.f) {
			owner->setPhysic(true);
			timer = 0.f;
		}
	}
	else {
		owner->AABB->removeIgnoredLayer(TagLayerManager::Instance().getLayer("Player"));
		owner->AABB->removeIgnoredLayer(TagLayerManager::Instance().getLayer("Box"));
	}

	auto* player = GameManager::instance().currentPlayer;
	if (!player) return;

	bool isGravityFlipped = player->getComponent<PlayerController>()->isGravityFlipped;

	if (!flipped && isGravityFlipped) {
		// Zwyk³a rotacja: X = 0, Y = 90, Z = -90
		glm::vec3 eulerRotation = glm::radians(glm::vec3(0.0f, 90.0f, -90.0f));
		owner->transform.setLocalRotation(glm::quat(eulerRotation));
		flipped = true;
	}
	else if (flipped && !isGravityFlipped) {
		// Oryginalna rotacja: X = 0, Y = 90, Z = 90
		glm::vec3 eulerRotation = glm::radians(glm::vec3(0.0f, 90.0f, 90.0f));
		owner->transform.setLocalRotation(glm::quat(eulerRotation));
		flipped = false;
	}
}

void Platform::onStayCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		owner->AABB->addIgnoredLayer(TagLayerManager::Instance().getLayer("Player"));
		timer = 0.3f;
	}

	if (other->getTagName() == "Box" && !flipped) {
		owner->AABB->addIgnoredLayer(TagLayerManager::Instance().getLayer("Box"));
		timer = 0.3f;
	}
}

void Platform::onExitCollisionLogic(Node* other)
{

}

void Platform::onStayCollision(Node* other)
{
	
}
