#include "Rigidbody.h"
#include "../Gameplay/GameMath.h"
#include "../Gameplay/PlayerController.h"
#include "../Basic/Node.h"
#include "../Gameplay/GroundObject.h"
#include "../System/PhysicsSystem.h"

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
	//isGrounded = false;

	//if (isGrounded) std::cout << owner->getName() << " stoi na pod³odze" << std::endl;
	//else std::cout << owner->getName() << " nie stoi na pod³odze" << std::endl;	
	
	if (is_static) {
        return;
    }

	// ground detection
	bool isGravityFlipped = false;
	glm::vec3 position = owner->transform.getGlobalPosition();
	glm::vec4 down = glm::vec4(0.f, -1.f, 0.f, 0.f);
	glm::vec4 up = glm::vec4(0.f, 1.f, 0.f, 0.f);
	float length = owner->transform.getLocalScale().y / 2.f + 0.02f;
	float width = owner->transform.getLocalScale().x / 2.f - 0.15f;
	std::vector<Node*> nodes;

	if (owner->getTagName() == "Player") {
		PlayerController* player = owner->getComponent<PlayerController>();
		if (player->isGravityFlipped) {
			std::swap(down, up);
			isGravityFlipped = true;
		}
	}

	std::vector<Ray> groundRays = {
		Ray{position + glm::vec3(-width, 0.f, 0.f), down},
		Ray{position, down},
		Ray{position + glm::vec3(width, 0.f, 0.f), down}
	};
	std::vector<Ray> ceilingRays = {
		Ray{position + glm::vec3(-width, 0.f, 0.f), up},
		Ray{position, up},
		Ray{position + glm::vec3(width, 0.f, 0.f), up}
	};

	groundUnderneath = false;
	ceilingAbove = false;

	nodes.clear();
	if (PhysicsSystem::instance().rayCast(groundRays, nodes, length)) {
		if (!(nodes.size() == groundRays.size() && nodes[groundRays.size() - 1] == owner)) {
			groundUnderneath = true;
		}
	}

	nodes.clear();
	if (PhysicsSystem::instance().rayCast(ceilingRays, nodes, length)) {
		if (!(nodes.size() == ceilingRays.size() && nodes[ceilingRays.size() - 1] == owner)) {
			ceilingAbove = true;
		}
	}

	if (startPos == glm::vec3(0.f)) startPos = owner->transform.getLocalPosition();

    float smoothingFactor = 10.0f;

	// horizontal movement
    velocityX = glm::mix(velocityX, targetVelocityX, smoothingFactor * deltaTime);

	// vertical movement
	if (overrideVelocityY) {
		velocityY = glm::mix(velocityY, targetVelocityY, smoothingFactor * 0.5f * deltaTime);
		velocityY += gravity * deltaTime;
	}
	else if (velocityY > 0.f && groundUnderneath && isGravityFlipped) {
		velocityY = 0.0f;
	}
	else if (velocityY < 0.f && ceilingAbove && isGravityFlipped) {
		velocityY = 0.f;
	}
	else if (velocityY < 0.f && groundUnderneath && !isGravityFlipped) {
		velocityY = 0.0f;
	}
	else if (velocityY > 0.f && ceilingAbove && !isGravityFlipped) {
		velocityY = 0.f;
	}
	else if (useGravity) {
		velocityY += gravity * deltaTime;
	}

	// veltical movement limit
	if (velocityY < -25.f || velocityY > 25.f) {
		velocityY = GameMath::Clamp(velocityY, -25.f, 25.f);
	}

	position = owner->transform.getLocalPosition();
	velocityDeltaX = position.x - (position.x + velocityX * deltaTime);
	velocityDeltaY = position.y - (position.y + velocityY * deltaTime);
    position += glm::vec3(velocityX * deltaTime, velocityY * deltaTime, 0.f);

	if (lockPositionX) position.x = startPos.x;
	if (lockPositionY) position.y = startPos.y;
	if (lockPositionZ) position.z = startPos.z;

    owner->transform.setLocalPosition(position);

	if (!isGrounded)
	{
		timer -= deltaTime;
		if (timer <= 0.0f)
			velocityYResetted = false;
	}

	overrideVelocityY = false;

	//std::cout << owner->getName() << " stoi na ziemi: " << groundUnderneath << ", dotyka sufitu: " << ceilingAbove << std::endl;
}

// collission with another rigidbody
void Rigidbody::onCollision(Node* other)
{
	if (is_static) {
		return;
	}
	
	Rigidbody* rb = other->getComponent<Rigidbody>();

    if (!rb) return;
	if (rb->is_static) return;

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
	/*if (other->getTagName() == "Player") {
		if (other->getComponent<PlayerController>()->isGrounded) {
			velocityY = 0.f;
		}
	}
	else if (other->getLayerName() == "Floor" || other->getLayerName() == "Platform") {
		if (!overrideVelocityY) {
			velocityY = 0.f;
		}
    }*/
}

void Rigidbody::onExitCollision(Node* other)
{
	if (is_static) {
		return;
	}
	
	Rigidbody* rb = other->getComponent<Rigidbody>();

    if (!rb) return;

	rb->targetVelocityX = 0.f;
}
