#include "Scale.h"
#include "../../Basic/Node.h"
#include "../RegisterScript.h"
#include "../GameMath.h"
#include "../PlayerController.h"
#include "../../System/PhysicsSystem.h"

REGISTER_SCRIPT(Scale);

void Scale::onAttach(Node* owner)
{
    this->owner = owner;
    std::cout << "Scale::onAttach::" << owner->name << std::endl;
    startPos1 = owner->transform.getLocalPosition();
    if (secondScale != nullptr) startPos2 = secondScale->transform.getLocalPosition();
}

void Scale::onDetach()
{
    std::cout << "Scale::onDetach::" << owner->name << std::endl;
    owner = nullptr;
}

void Scale::onStart()
{
    std::cout << "start szalek -----------------------------------------------------------" << std::endl;

    rb = owner->getComponent<Rigidbody>();
    rb->dragY = 1.f;
    startPos1 = owner->transform.getLocalPosition();
    startPos2 = secondScale->transform.getLocalPosition();

    if (moveHorizontally) {
        rb->lockPositionY = true;
    }
    else {
        rb->lockPositionX = true;
    }
    rb->lockPositionZ = true;
}

void Scale::onUpdate(float deltaTime)
{
    if (secondScale == nullptr) return;
    //else if (startPos2 == glm::vec3(0.f)) startPos2 = secondScale->transform.getLocalPosition();

    /*if (setStartPos) {
        startPos1 = owner->transform.getLocalPosition();
        startPos2 = secondScale->transform.getLocalPosition();
        setStartPos = false;
    }*/

    // Wykrywanie gracza (promieñ w górê)
    glm::vec3 position = owner->transform.getGlobalPosition();
    glm::vec4 up = glm::vec4(0.f, 1.f, 0.f, 0.f);
    float length = owner->transform.getLocalScale().y / 2.f + 0.01f;
    float width = owner->transform.getLocalScale().x / 2.f - 0.05f;

    std::vector<Ray> upRays = {
        Ray{position + glm::vec3(-width, 0.f, 0.f), up},
        Ray{position, up},
        Ray{position + glm::vec3(width, 0.f, 0.f), up}
    };

    std::vector<RayCastHit> hitNodes;
    bool playerOnTop = false;
    if (PhysicsSystem::instance().rayCast(upRays, hitNodes, length, owner)) {
		if (hitNodes.size() > 0) {
			for (RayCastHit r : hitNodes) {
				if (r.node->getTagName() == "Player" || r.node->getTagName() == "Box") {
					playerOnTop = true;
					break;
				}
			}
		}
    }

    glm::vec3 pos1 = owner->transform.getLocalPosition();
    glm::vec3 pos2 = secondScale->transform.getLocalPosition();

    if (!playerOnTop && returnToPosition) {
        glm::vec3 newPos1 = mix(pos1, startPos1, 0.5f * deltaTime);
        glm::vec3 newPos2 = mix(pos2, startPos2, 0.5f * deltaTime);

        owner->transform.setLocalPosition(newPos1);
        secondScale->transform.setLocalPosition(newPos2);
    }

    float offset = startPos1.y - pos1.y;
    glm::vec3 newPos2 = startPos2 + glm::vec3(0.f, offset, 0.f);

    secondScale->transform.setLocalPosition(newPos2);
    
    //// Ruch pionowy
    //float maxDisplacement = 1.0f; // maksymalna ró¿nica wysokoœci miêdzy szalkami
    //glm::vec3 pos1 = owner->transform.getLocalPosition();
    //glm::vec3 pos2 = secondScale->transform.getLocalPosition();
    //float delta = pos1.y - pos2.y;

    //float moveSpeed = 15.0f; // prêdkoœæ celowana
    //if (playerOnTop) {
    //    if (delta < maxDisplacement) {
    //        rb->targetVelocityY = -moveSpeed;
    //        Rigidbody* rb2 = secondScale->getComponent<Rigidbody>();
    //        if (rb2) rb2->targetVelocityY = moveSpeed;
    //    }
    //    else {
    //        rb->targetVelocityY = 0.f;
    //        Rigidbody* rb2 = secondScale->getComponent<Rigidbody>();
    //        if (rb2) rb2->targetVelocityY = 0.f;
    //    }
    //}
    //else {
    //    // Automatyczny powrót do poziomu
    //    float restoreSpeed = 1.5f;
    //    if (delta > 0.05f) {
    //        rb->targetVelocityY = restoreSpeed;
    //        Rigidbody* rb2 = secondScale->getComponent<Rigidbody>();
    //        if (rb2) rb2->targetVelocityY = -restoreSpeed;
    //    }
    //    else if (delta < -0.05f) {
    //        rb->targetVelocityY = -restoreSpeed;
    //        Rigidbody* rb2 = secondScale->getComponent<Rigidbody>();
    //        if (rb2) rb2->targetVelocityY = restoreSpeed;
    //    }
    //    else {
    //        rb->targetVelocityY = 0.f;
    //        Rigidbody* rb2 = secondScale->getComponent<Rigidbody>();
    //        if (rb2) rb2->targetVelocityY = 0.f;
    //    }
    //}
}


void Scale::onEnd() {}

void Scale::onStayCollision(Node* other) {}

void Scale::onExitCollision(Node* other) {}
