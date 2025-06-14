#include "TutorialManager.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "../System/GuiManager.h"
#include "GameManager.h"
#include "GasWall.h"

REGISTER_SCRIPT(TutorialManager);

void TutorialManager::onAttach(Node* owner)
{
	this->owner = owner;
}

void TutorialManager::onDetach()
{
	owner = nullptr;
}

void TutorialManager::onStart()
{
	scoreText = GuiManager::Instance().findText(0);
	runTimeText = GuiManager::Instance().findText(1);
	deathCountText = GuiManager::Instance().findText(2);

	scoreText->visible = false;
	runTimeText->visible = false;
	deathCountText->visible = false;
}

void TutorialManager::onUpdate(float deltaTime)
{
	if (showStats) {
		timer += deltaTime;

		if (timer >= 0.5f && counter <= 8) {
			timer = 0.f;
			counter++;
			scoreText->visible = !scoreText->visible;
			runTimeText->visible = !runTimeText->visible;
			deathCountText->visible = !deathCountText->visible;
		}
		else if (counter == 8) {
			showStats = false;
			scoreText->visible = true;
			runTimeText->visible = true;
			deathCountText->visible = true;
		}
	}
}

void TutorialManager::onCollisionLogic(Node* other)
{
	if (other->getTagName() == "Player") {
		showStats = true;
		GameManager::instance->tutorialActive = false;

		glm::vec3 emitterPos = emitter->transform.getGlobalPosition();
		GameManager::instance->emitter->getComponent<GasWall>()->spreadQueue = std::queue<glm::ivec2>();

		GameManager::instance->emitter->getComponent<GasWall>()->spreadQueue.push(emitterPos);

		/*int index = GameManager::instance->emitter->tail - 1;
		
		//GameManager::instance->emitter->particles[index].position = glm::vec4(emitterPos, 0.f);*/

		//GameManager::instance->emitter = dynamic_cast<InstanceManager*>(emitter);
		//emitter->getComponent<GasWall>()->spreading = true;
	}
}
