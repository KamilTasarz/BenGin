#include "Rigidbody.h"

#include "../Basic/Node.h"

Rigidbody::Rigidbody(float mass, float gravity, bool isStatic, bool useGravity, bool lockPositionX, bool lockPositionY, bool lockPositionZ)
{
	this->mass = mass;
	this->gravity = gravity;
	this->is_static = isStatic;
	this->useGravity = useGravity;
	this->lockPositionX = lockPositionX;
	this->lockPositionY = lockPositionY;
	this->lockPositionZ = lockPositionZ;
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

void Rigidbody::onStart()
{
	velocityX = 0.f;
	targetVelocityX = 0.f;
	velocityY = 0.f;
	targetVelocityY = 0.f;

	startPos = owner->transform.getLocalPosition();
}

void Rigidbody::onUpdate(float deltaTime)
{
    if (is_static) {
        return;
    }

	if (startPos == glm::vec3(0.f)) startPos = owner->transform.getLocalPosition();

    float smoothingFactor = 10.0f;
    velocityX = glm::mix(velocityX, targetVelocityX, smoothingFactor * deltaTime);

	
	if (useGravity) {
		velocityY += gravity * deltaTime;
	}
	else {
		velocityY = glm::mix(velocityY, targetVelocityY, smoothingFactor * deltaTime);
	}

    if (velocityY < -20.f) {
		velocityY = -20.f;
    }

    glm::vec3 position = owner->transform.getLocalPosition();
    position += glm::vec3(velocityX * deltaTime, velocityY * deltaTime, 0.f);

	if (lockPositionX) position.x = startPos.x;
	if (lockPositionY) position.y = startPos.y;
	if (lockPositionZ) position.z = startPos.z;

    owner->transform.setLocalPosition(position);
}

// collission with another rigidbody
void Rigidbody::onCollision(Node* other)
{
    Rigidbody* rb = other->getComponent<Rigidbody>();

    if (!rb) return;

    float averageVelocityX = (velocityX * mass + rb->velocityX * rb->mass) / (mass + rb->mass);
	float averageVelocityY = (velocityY * mass + rb->velocityY * rb->mass) / (mass + rb->mass);

    velocityX = averageVelocityX;
    rb->velocityX = averageVelocityX;

	velocityY = averageVelocityY;
	rb->velocityY = averageVelocityY;

	/*float massSum = mass + rb->mass;
	float massRatio = mass / massSum;

	targetVelocityX *= massRatio;*/

	//// Apply friction
	//float friction = 0.5f;
	//velocityX *= (1.f - friction);
	//rb->velocityX *= (1.f - friction);
	//// Apply restitution
	//float restitution = 0.5f;
	//velocityY *= -restitution;
	//rb->velocityY *= -restitution;
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
