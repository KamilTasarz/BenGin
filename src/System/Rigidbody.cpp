#include "Rigidbody.h"

#include "../Basic/Node.h"

Rigidbody::Rigidbody(float mass, float gravity, bool isStatic)
{
	this->mass = mass;
	this->gravity = gravity;
	this->is_static = isStatic;
	name = "Rigidbody";
}

void Rigidbody::onAttach(Node* node)
{
	owner = node;
}

void Rigidbody::onDetach()
{
	owner = nullptr;
}

void Rigidbody::onUpdate(float deltaTime)
{
    if (is_static) {
        return;
    }

    float smoothingFactor = 10.0f;
    velocityX = glm::mix(velocityX, targetVelocityX, smoothingFactor * deltaTime);

    velocityY += gravity * deltaTime;
    if (velocityY < -20.f) {
		velocityY = -20.f;
    }

    glm::vec3 position = owner->transform.getLocalPosition();
    position += glm::vec3(velocityX * deltaTime, velocityY * deltaTime, 0.f);

    owner->transform.setLocalPosition(position);
}

// collission with another rigidbody
void Rigidbody::onCollision(Node* other)
{
    Rigidbody* rb = other->getComponent<Rigidbody>();

    if (!rb) return;

    float averageVelocity = (velocityX * mass + rb->velocityX * rb->mass) / (mass + rb->mass);

    velocityX = averageVelocity;
    rb->velocityX = averageVelocity;
}

void Rigidbody::onStayCollision(Node* other)
{
    if (other->getLayerName() == "Floor") {
        velocityY = 0.f;
    }
}

void Rigidbody::onExitCollision(Node* other)
{
    Rigidbody* rb = other->getComponent<Rigidbody>();

    if (!rb) return;

	rb->targetVelocityX = 0.f;
}
