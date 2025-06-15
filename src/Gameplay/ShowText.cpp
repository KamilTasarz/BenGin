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
	textObjectSecond = GuiManager::Instance().findText(10);

	if (!textObject) {
		std::cout << "ShowText::onStart: TextObject not found!" << std::endl;
		return;
	}

	textObject->alignment = 1;
	textObjectSecond->alignment = 1;

	isWriting = false;
}

void ShowText::onUpdate(float deltaTime)
{
	if (!isWriting && !isDeleting) return;

	timer += deltaTime;

	if (timer >= speed && isDeleting) {
		timer = 0.f + GameMath::RandomFloat(-speed / 2, speed / 2);

		if (!textObjectSecond->value.empty()) {
			textObjectSecond->value.pop_back();

			auto* audio = ServiceLocator::getAudioEngine();
			audio->PlaySFX(audio->writing, GameManager::instance->sfxVolume * 80.f);
		}
		else if (!textObject->value.empty()) {
			textObject->value.pop_back();

			auto* audio = ServiceLocator::getAudioEngine();
			audio->PlaySFX(audio->writing, GameManager::instance->sfxVolume * 80.f);
		}
		else {
			isDeleting = false;
		}
	}
	else if (timer >= speed && isWriting) {
		timer = 0.f + GameMath::RandomFloat(-0.05, 0.05);

		if (!textChars.empty()) {
			char c = textChars.front();
			textChars.erase(textChars.begin());

			if (!writingFirstDone) {
				textObject->value += c;
			}
			else {
				textObjectSecond->value += c;
			}

			if (c != ' ') {
				auto* audio = ServiceLocator::getAudioEngine();
				audio->PlaySFX(audio->writing, GameManager::instance->sfxVolume * 80.f);
			}
		}
		else {
			if (!writingFirstDone && !textSecond.empty()) {
				// Pierwsza linijka zakoñczona — zacznij drug¹
				writingFirstDone = true;
				for (char c : textSecond) {
					textChars.push_back(c);
				}
			}
			else {
				isWriting = false;
			}
		}
	}
}

void ShowText::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		if (!isWriting && !entered) {
			isWriting = true;
			entered = true;
			writingFirstDone = false;

			textObject->value = "";
			textObjectSecond->value = "";

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



