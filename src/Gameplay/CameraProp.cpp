#include "CameraProp.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "GameManager.h"

REGISTER_SCRIPT(CameraProp);

void CameraProp::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "CameraProp::onAttach::" << owner->name << std::endl;
}

void CameraProp::onDetach()
{
	std::cout << "CameraProp::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void CameraProp::onStart()
{
	std::cout << "CameraProp::onStart::" << owner->name << std::endl;
	// Initialize camera properties here if needed
}

void CameraProp::onUpdate(float deltaTime)
{
    // Pozycja gracza i kamery
    glm::vec3 playerPos = GameManager::instance->currentPlayer->transform.getGlobalPosition();
    glm::vec3 cameraPos = owner->transform.getGlobalPosition();

    // Ró¿nice pozycji
    float diffX = playerPos.x - cameraPos.x;
    float diffY = playerPos.y - cameraPos.y;

    // Przekszta³æ ró¿nice na k¹ty w stopniach (z czu³oœci¹ i ograniczeniem)
    float desiredYDeg = glm::clamp(diffX * 9.f, -90.0f, 90.0f); // obrót w lewo/prawo
    float desiredXDeg = glm::clamp(-diffY * 9.f, -45.0f, 45.0f) - 45.f; // obrót w górê/dó³ (minus, ¿eby obraca³o poprawnie)

    // Konwertuj na radiany
    float desiredYRad = glm::radians(desiredYDeg);
    float desiredXRad = glm::radians(desiredXDeg);

    // Obecna rotacja jako kwaternion
    glm::quat currentQuat = owner->transform.getLocalRotation();
    glm::vec3 currentEuler = glm::eulerAngles(currentQuat);

    // Wyg³adzanie rotacji
    float smoothedY = glm::mix(currentEuler.y, desiredYRad, deltaTime * 3.f);
    float smoothedX = glm::mix(currentEuler.x, desiredXRad, deltaTime * 3.f);

    // Nowa rotacja (uwaga: kolejnoœæ rotacji ma znaczenie przy du¿ych k¹tach)
    glm::quat newQuat = glm::quat(glm::vec3(smoothedX, smoothedY, 0.f));

    // Ustawienie nowej rotacji
    owner->transform.setLocalRotation(newQuat);
}

