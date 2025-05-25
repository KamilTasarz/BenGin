#include "Rigidbody.h"
#include "../Gameplay/GameMath.h"
#include "../Gameplay/PlayerController.h"
#include "../Basic/Node.h"
#include "../Gameplay/GroundObject.h"
#include "../System/PhysicsSystem.h"
#include "../Component/BoundingBox.h"

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

	float lastVelocityX = velocityX;
	float lastVelocityY = velocityY;

	// ground detection
	bool isGravityFlipped = false;
	//glm::vec3 position = owner->transform.getGlobalPosition();
	glm::vec3 position = (owner->AABB->max_point_world + owner->AABB->min_point_world) / 2.f;
	glm::vec4 down = glm::vec4(0.f, -1.f, 0.f, 0.f);
	glm::vec4 up = glm::vec4(0.f, 1.f, 0.f, 0.f);
	//float length = owner->transform.getLocalScale().y / 2.f + 0.03f;
	float length = (owner->AABB->max_point_world.y - owner->AABB->min_point_world.y) / 2.f + 0.03f;
	float width = (owner->AABB->max_point_world.x - owner->AABB->min_point_world.x) / 2.f - 0.15f;
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
	scaleUnderneath = false;
	ceilingAbove = false;

	nodes.clear();
	if (PhysicsSystem::instance().rayCast(groundRays, nodes, length, owner)) {
		if (nodes.size() > 0) {
			for (Node* node : nodes) {
				if (node->getLayerName() == "Scale") {
					if (node->getComponent<Rigidbody>()->groundUnderneath) {
						groundUnderneath = true;
					}
					else {
						scaleUnderneath = true;
					}
				}
				if (node->getLayerName() != "Scale") {
					groundUnderneath = true;
				}
			}
		}
	}

	nodes.clear();
	if (PhysicsSystem::instance().rayCast(ceilingRays, nodes, length, owner)) {
		if (nodes.size() > 0) {
			ceilingAbove = true;
		}
	}

	if (startPos == glm::vec3(0.f)) startPos = owner->transform.getLocalPosition();

	// horizontal movement
	velocityX = glm::mix(velocityX, targetVelocityX, smoothingFactor * deltaTime);
	velocityX *= (1.0f - drag * deltaTime);

	// vertical movement
	if (overrideVelocityY) {
		velocityY = glm::mix(velocityY, targetVelocityY, smoothingFactor * 0.5f * deltaTime);
		velocityY += gravity * deltaTime;
		velocityY *= (1.0f - drag * deltaTime);
	}
	else if (scaleUnderneath && !groundUnderneath && velocityY < 0.f) {
		velocityY = -0.5f;
		
		PlayerController* player = owner->getComponent<PlayerController>();
		if (player) {
			if (player->virusType == "black") {
				velocityY = -5.f;
			}
		}
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
		velocityY *= (1.0f - drag * deltaTime);
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

	velocityDeltaX = velocityX - lastVelocityX;
	velocityDeltaY = velocityY - lastVelocityY;

	overrideVelocityY = false;
	overrideVelocityX = false;

	targetVelocityX = 0.f;
	targetVelocityY = 0.f;

	//std::cout << owner->getName() << " stoi na ziemi: " << groundUnderneath << ", dotyka sufitu: " << ceilingAbove << std::endl;
}

// collission with another rigidbody
void Rigidbody::onCollision(Node* other) {
	//if (is_static) return;

	//Rigidbody* rb = other->getComponent<Rigidbody>();
	//if (!rb || rb->is_static) return;

	//// Relative velocity
	//glm::vec2 relativeVelocity(velocityX - rb->velocityX, velocityY - rb->velocityY);

	//// Approximate normal (we assume axis-aligned for simplicity)
	//glm::vec2 collisionNormal = glm::normalize(relativeVelocity);
	//if (glm::length(collisionNormal) == 0.0f) return;

	//// Relative velocity along the normal
	//float velAlongNormal = glm::dot(relativeVelocity, collisionNormal);

	//// Ignore if objects are separating
	//if (velAlongNormal > 0) return;

	//// Coefficient of restitution (bounciness)
	//float restitution = 0.2f; // 0 = inelastic, 1 = elastic

	//// Calculate impulse scalar
	//float invMassA = (mass > 0.f ? 1.f / mass : 0.f);
	//float invMassB = (rb->mass > 0.f ? 1.f / rb->mass : 0.f);
	//float impulseScalar = -(1 + restitution) * velAlongNormal / (invMassA + invMassB);

	//// Apply impulse to each object (change velocity)
	//glm::vec2 impulse = impulseScalar * collisionNormal;

	//// Change velocities based on mass
	//velocityX += impulse.x * invMassA;
	//velocityY += impulse.y * invMassA;

	//rb->velocityX -= impulse.x * invMassB;
	//rb->velocityY -= impulse.y * invMassB;
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
