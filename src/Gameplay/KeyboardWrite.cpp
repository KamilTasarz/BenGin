#include "KeyboardWrite.h"
#include "RegisterScript.h"
#include "GameManager.h"
#include "../System/GuiManager.h"

REGISTER_SCRIPT(KeyboardWrite);

void KeyboardWrite::onStart()
{
	textObject = GuiManager::Instance().findText(53);
	textObject->value = GameManager::instance().player_name;
	text = GameManager::instance().player_name;

}

void KeyboardWrite::onUpdate(float deltaTime)
{
	if (isActive) {

		timer -= deltaTime;

		if (timer < 0.f) {
			timer = buffer;
			lastSign = !lastSign;
			if (lastSign) {
				text += "|"; // cursor
			}
			else {
				text.pop_back(); // remove cursor
			}
		}



		// Handle keyboard input for writing text
		for (int key = GLFW_KEY_A; key <= GLFW_KEY_Z; ++key) {
			if (glfwGetKey(ServiceLocator::getWindow()->window, key) == GLFW_PRESS) {
				if (!pressedKeys.contains(key)) {
					if (lastSign) {
						text.pop_back(); // remove cursor
						lastSign = false;
					}
					text += static_cast<char>(key + 'A' - GLFW_KEY_A);
					timer = buffer;
					lastSign = true;
					if (lastSign) {
						text += "|"; // cursor
					}
					//isPressed = true;
					pressedKeys.insert(key);
				}
			}
			else {
				//isPressed = false;
				pressedKeys.erase(key);
			}
		}
		for (int key = GLFW_KEY_0; key <= GLFW_KEY_9; ++key) {
			if (glfwGetKey(ServiceLocator::getWindow()->window, key) == GLFW_PRESS) {
				if (!pressedKeys.contains(key)) {
					if (lastSign) {
						text.pop_back(); // remove cursor
						lastSign = false;
					}
					text += static_cast<char>(key + '0' - GLFW_KEY_0);
					//isPressed = true;
					pressedKeys.insert(key);
					timer = buffer;
					lastSign = true;
					if (lastSign) {
						text += "|"; // cursor
					}
				}
			}
			else {
				pressedKeys.erase(key);
				//isPressed = false;
			}
		}
		if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			if (!isPressed) {
				if (lastSign) {
					text.pop_back(); // remove cursor
					lastSign = false;
				}
				text += ' ';
				isPressed = true;
				timer = buffer;
				lastSign = true;
				if (lastSign) {
					text += "|"; // cursor
				}
			}
		}
		else {
			isPressed = false;
		}

		if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
			if (!isPressed_back) {
				if (lastSign) {
					text.pop_back(); // remove cursor
					lastSign = false;
				}
				if (!text.empty()) {
					text.pop_back();
				}
				isPressed_back = true;
				timer = buffer;
				lastSign = true;
				if (lastSign) {
					text += "|"; // cursor
				}
			}
		}
		else {
			isPressed_back = false;
		}

		if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_ENTER) == GLFW_PRESS) {
			if (!isPressed_ent) {
				if (lastSign) {
					text.pop_back(); // remove cursor
					lastSign = false;
				}
				GameManager::instance().player_name = text;
				isActive = false;
				lastSign = false;
				isPressed_ent = true;
			}
		}
		else {
			isPressed_ent = false;
		}
		textObject->value = text;
	}
}
