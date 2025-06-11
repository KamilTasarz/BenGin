#include "TVManager.h"
#include "../RegisterScript.h"
#include "../../Basic/Node.h"
#include "../RotateObject.h"

REGISTER_SCRIPT(TVManager);

void TVManager::onAttach(Node* owner) {
	this->owner = owner;
}

void TVManager::onDetach() {
	owner = nullptr;
}

void TVManager::onStart() {
	tvStartPos = tv->transform.getLocalPosition();
	cheeseStartPos = cheese->transform.getLocalPosition();

	tvAI = tv->getComponent<NPC>();
}

void TVManager::onUpdate(float deltaTime) {
	if (!tvAI) return;

	if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_R) == GLFW_PRESS) {
		isFleeing = false;
		tv->transform.setLocalPosition(tvStartPos);
		cheese->transform.setLocalPosition(cheeseStartPos);
		cheese->getChildById(0)->getComponent<RotateObject>()->stopRotation = true;
		currentVelocity = glm::vec3(0.f, 0.f, 3.f);
		targetVelocity = glm::vec3(0.f);
		tvAI->isCatched = false;
		//tv->setPhysic(true);
	}

	if (tvAI->isCatched) {
		float velX = tv->getComponent<Rigidbody>()->velocityX;
		
		targetVelocity = glm::vec3(0.f, 0.f, 0.f);
		if (velX >= 0.f) targetVelocity = glm::vec3(3.f, 12.f, 3.f);
		else if (velX < 0.f) targetVelocity = glm::vec3(-3.f, 12.f, 3.f);

		cheese->getChildById(0)->getComponent<RotateObject>()->stopRotation = false;

		//tv->setPhysic(false);
		tvAI->isCatched = false;
		isFleeing = true;
	}
	else if (isFleeing) {
		currentVelocity += (targetVelocity - currentVelocity) * deltaTime;
		glm::vec3 newPos = tv->transform.getLocalPosition() + currentVelocity * deltaTime;

		tv->transform.setLocalPosition(newPos);
	}
	else {
		glm::vec currentTVPos = tv->transform.getLocalPosition()/* / tv->transform.getLocalScale()*/;
		glm::vec3 movingOffset = currentTVPos - tvStartPos;
		cheese->transform.setLocalPosition(cheeseStartPos + movingOffset);

		float velX = tv->getComponent<Rigidbody>()->velocityX;
		glm::quat targetRotation = glm::quat(glm::vec3(0.f, 0.f, glm::radians(-velX * 2.f)));

		// Pobierz aktualn¹ rotacjê
		glm::quat currentRotation = owner->transform.getLocalRotation();

		// Interpoluj miêdzy aktualn¹ a docelow¹
		float smoothSpeed = 1.0f; // im wiêksza, tym szybciej dochodzi
		glm::quat newRotation = glm::slerp(currentRotation, targetRotation, deltaTime * smoothSpeed);

		// Ustaw wyg³adzon¹ rotacjê
		owner->transform.setLocalRotation(newRotation);
	}
}
