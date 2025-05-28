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

	length = (owner->AABB->max_point_world.y - owner->AABB->min_point_world.y) / 2.f;
	width = (owner->AABB->max_point_world.x - owner->AABB->min_point_world.x) / 2.f;
}

void Rigidbody::onUpdate(float deltaTime)
{
	if (is_static) {
        return;
    }

	float lastVelocityX = velocityX;
	float lastVelocityY = velocityY;

	// ground detection
	glm::vec3 position = (owner->AABB->max_point_world + owner->AABB->min_point_world) / 2.f;

	groundUnderneath = false;
	scaleUnderneath = false;
	ceilingAbove = false;
	isPushing = false;

	glm::vec4 down = glm::vec4(0.f, -1.f, 0.f, 0.f);
	if(isGravityFlipped) down *= -1;

	std::vector<Node*> nodes;

	std::vector<Ray> Rays = {
		Ray{position + glm::vec3(-width + 0.05f, 0.f, 0.f), down},
		Ray{position, down},
		Ray{position + glm::vec3(width - 0.05f, 0.f, 0.f), down}
	};
	//std::vector<Ray> ceilingRays = {
	//	Ray{position + glm::vec3(-width, 0.f, 0.f), up},
	//	Ray{position + glm::vec3(width, 0.f, 0.f), up}
	//};

	nodes.clear();
	if (PhysicsSystem::instance().rayCast(Rays, nodes, length + 0.2f, owner)) {
		if (nodes.size() > 0) {

			groundUnderneath = true;

			//for (Node* node : nodes) {
			//	//if (owner->getComponent<PlayerController>()) std::cout << "Gracz dotyka: " << node->getName() << ", warstwa: " << node->getLayerName() << std::endl;

			//	glm::vec3 nodePos = node->transform.getGlobalPosition();

			//	if (nodePos.y < position.y) {
			//		if (node->getLayerName() == "Scale") {
			//			if (node->getComponent<Rigidbody>()->groundUnderneath)
			//				groundUnderneath = true;
			//			else
			//				scaleUnderneath = true;
			//		}
			//		else {
			//			groundUnderneath = true;
			//		}
			//	}
			//	/*else if (nodePos.y > position.y) {
			//		ceilingAbove = true;
			//	}*/
			//}
		}
	}

	/*if (isGravityFlipped) {
		std::swap(groundUnderneath, ceilingAbove);
	}*/

 	if (isPlayer) {
		//std::cout << "kierunek: " << side.x << ", " << side.y << ", " << side.z << ", " << side.w << std::endl;

		Ray ray = Ray{ position, side };

		nodes.clear();
		if (PhysicsSystem::instance().rayCast(ray, nodes, width + 0.2f, owner)) {
			if (nodes.size() > 0) {
				isPushing = true;
				//std::cout << "Gracz pcha obiekt: " << nodes[0]->getName() << std::endl;
			}
		}
	}

	// horizontal movement
	velocityX = glm::mix(velocityX, targetVelocityX, smoothingFactor * deltaTime);
	velocityX *= (1.0f - drag * deltaTime);

	// vertical movement
	if (overrideVelocityY) {
		velocityY = glm::mix(velocityY, targetVelocityY, smoothingFactor * 0.5f * deltaTime);
		velocityY += gravity * deltaTime;
		velocityY *= (1.0f - drag * deltaTime);
	}
	/*else if (scaleUnderneath && !groundUnderneath && velocityY < 0.f) {
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
	}*/
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
	if (is_static || !other->is_physic_active) return;

	float sep = 1.f;

	Rigidbody* rb = other->getComponent<Rigidbody>();
	if (rb && !rb->is_static) {
		sep = .5f;

		/*if (tryingToMoveAwayFrom(other)) {
			sep = 1.f;
		}*/
	}

	owner->AABB->separate(other->AABB, sep);

	if (rb && !rb->is_static && owner->AABB->collison == 1) {
		float mass2 = rb->mass;
		float velx2 = rb->velocityX;
		float velx = (velocityX * mass + velx2 * mass2) / (mass + mass2);
		velocityX = velx;
		rb->velocityX = velx;
	}
	else if (rb && !rb->is_static && owner->AABB->collison == 2) {
		float mass2 = rb->mass;
		float vely2 = rb->velocityY;
		float vely = (velocityY * mass + vely2 * mass2) / (mass + mass2);
		velocityY = vely;
		rb->velocityY = vely;
	}
	else if (!(rb && !rb->is_static) && owner->AABB->collison == 1) {
		velocityX = 0.f;
	} 
	else if (!(rb && !rb->is_static) && owner->AABB->collison == 2) {
		velocityY = 0.f;
	}
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

bool Rigidbody::tryingToMoveAwayFrom(Node* other) {
	glm::vec3 diff = owner->transform.getLocalPosition() - other->transform.getLocalPosition();
	if (std::abs(diff.x) > std::abs(diff.y)) {
		return (diff.x < 0 && velocityX < 0) || (diff.x > 0 && velocityX > 0);
	}
	else {
		return (diff.y < 0 && velocityY < 0) || (diff.y > 0 && velocityY > 0);
	}
}
