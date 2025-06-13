#include "ShowText.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "../System/GuiManager.h"
#include "GameManager.h"
#include "GameMath.h"

REGISTER_SCRIPT(ShowText);

void ShowText::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "ShowText::onAttach::" << owner->name << std::endl;
}

void ShowText::onDetach()
{
	owner = nullptr;
}

void ShowText::onStart()
{
	//text = "Escape from the gas!";
	
	textObject = GuiManager::Instance().findText(9);
	if (!textObject) {
		std::cout << "ShowText::onStart: TextObject not found!" << std::endl;
		return;
	}

	//textObject->setActive(false);
	//textChars.clear();

	//for (char c : text) {
	//	textChars.push_back(c);
	//}

	//textObject->value = "";

	isWriting = false;
}

void ShowText::onUpdate(float deltaTime)
{
	if (!isWriting && !isDeleting) {
		return;
	}

	static float timer = -3.0f;
	timer += deltaTime;

	if (timer >= speed && isDeleting) {
		timer = 0.0f + GameMath::RandomFloat(-0.05, 0.05);

		if (!textObject->value.empty()) {
			textObject->value.pop_back();

			auto* audio = ServiceLocator::getAudioEngine();
			audio->PlaySFX(audio->writing, GameManager::instance->sfxVolume * 80.f);
		}
		if (textObject->value.empty()) {
			isDeleting = false;
			//textObject->setActive(false);
		}
	}
	else if (timer >= speed && isWriting) {
		timer = 0.0f + GameMath::RandomFloat(-0.05, 0.05);

		if (!textChars.empty()) {
			textObject->value += textChars.front();

			if (textChars.front() != ' ') {
				auto* audio = ServiceLocator::getAudioEngine();
				audio->PlaySFX(audio->writing, GameManager::instance->sfxVolume * 80.f);
			}

			textChars.erase(textChars.begin());
		}
		if (textChars.empty()) {
			isWriting = false;
			//textObject->setActive(false);
		}
	}
}

void ShowText::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		if (!isWriting && !entered) {
			isWriting = true;
			entered = true;
			//textObject->setActive(true);
			textObject->value = "";
			textChars.clear();

			for (char c : text) {
				textChars.push_back(c);
			}
		}
	}
}

void ShowText::onExitCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		isWriting = false;
		isDeleting = true;
	}
}



