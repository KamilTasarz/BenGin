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

	//float lastVelocityX = velocityX;
	//float lastVelocityY = velocityY;

	if (isBufferX) {
		velocityX = velocityXBuffer;
		isBufferX = false;
	}
	if (isBufferY) {
		velocityY = velocityYBuffer;
		isBufferY = false;
	}

	// ground detection
	glm::vec3 position = (owner->AABB->max_point_world + owner->AABB->min_point_world) / 2.f;

	groundUnderneath = false;
	//scaleUnderneath = false;
	//ceilingAbove = false;
	isPushing = false;

	glm::vec4 down = glm::vec4(0.f, -1.f, 0.f, 0.f);
	if(isGravityFlipped) down *= -1;

	std::vector<Node*> nodes;

	std::vector<Ray> Rays = {
		Ray{position + glm::vec3(-width + 0.05f, 0.f, 0.f), down},
		Ray{position, down},
		Ray{position + glm::vec3(width - 0.05f, 0.f, 0.f), down}
	};

	nodes.clear();
	if (PhysicsSystem::instance().rayCast(Rays, nodes, length + 0.2f, owner)) {
		if (nodes.size() > 0) {

			groundUnderneath = true;
		}
	}

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
		//velocityY += gravity * deltaTime;
		//velocityY *= (1.0f - drag * deltaTime);
	}

	if (useGravity && groundUnderneath) {
		velocityY += gravity * deltaTime * 0.2f;
	}
	else if (useGravity) {
		velocityY += gravity * deltaTime;
	}

	velocityY *= (1.0f - drag * deltaTime);

	// veltical movement limit
	if (velocityY < -25.f || velocityY > 25.f) {
		velocityY = GameMath::Clamp(velocityY, -25.f, 25.f);
	}

	position = owner->transform.getLocalPosition();
	glm::vec3 deltaPosition = glm::vec3(velocityX * deltaTime, velocityY * deltaTime, 0.f);
	position += deltaPosition;

	if (lockPositionX) position.x = startPos.x;
	if (lockPositionY) position.y = startPos.y;
	if (lockPositionZ) position.z = startPos.z;

    owner->transform.setLocalPosition(position);

	/*if (!isGrounded)
	{
		timer -= deltaTime;
		if (timer <= 0.0f)
			velocityYResetted = false;
	}*/

	//velocityDeltaX = velocityX - lastVelocityX;
	//velocityDeltaY = velocityY - lastVelocityY;

	overrideVelocityY = false;
	overrideVelocityX = false;

	targetVelocityX = 0.f;
	targetVelocityY = 0.f;

	//std::cout << owner->getName() << " stoi na ziemi: " << groundUnderneath << ", dotyka sufitu: " << ceilingAbove << std::endl;
}

void Rigidbody::onCollision(Node* other) {
}


void Rigidbody::onStayCollision(Node* other)
{
	if (!owner || !owner->AABB || !other || !other->AABB) {
		std::cerr << "onStayCollision: Null owner or other or their AABBs." << std::endl;
		return;
	}
	if (is_static || !other->is_physic_active) return;

	Rigidbody* rb_other = other->getComponent<Rigidbody>();

	// --- 1. Positional Correction (Separation) ---
	glm::vec3 owner_pos_before_sep = owner->transform.getLocalPosition();
	glm::vec3 other_pos_before_sep = (rb_other && !rb_other->is_static) ? other->transform.getLocalPosition() : glm::vec3(0);

	owner->AABB->separate(other->AABB, 1.0f);

	glm::vec3 owner_pos_after_sep = owner->transform.getLocalPosition();
	glm::vec3 owner_displacement_by_sep = owner_pos_after_sep - owner_pos_before_sep;

	if (rb_other && !rb_other->is_static) {

		glm::vec3 half_penetration = owner_displacement_by_sep * 0.5f;
		owner->transform.setLocalPosition(owner_pos_before_sep + half_penetration);
		other->transform.setLocalPosition(other_pos_before_sep - half_penetration);

		owner->forceUpdateSelfAndChild();
		other->forceUpdateSelfAndChild();
	}
	
	// --- 2. Velocity Response ---
	// (1 for X, 2 for Y)
	int collision_axis = owner->AABB->collison;

	if (rb_other && !rb_other->is_static) { // Dynamic vs Dynamic
		float m1 = mass;
		float m2 = rb_other->mass;
		float totalMass = m1 + m2;
		if (totalMass == 0) totalMass = .01f;

		if (collision_axis == 1) {
			float v1x = velocityX;
			float v2x = rb_other->velocityX;
			float final_vx = (m1 * v1x + m2 * v2x) / totalMass;

			velocityX = final_vx;
			rb_other->velocityX = final_vx;
		}
		else if (collision_axis == 2) {
			float v1y = velocityY;
			float v2y = rb_other->velocityY;
			float final_vy = (m1 * v1y + m2 * v2y) / totalMass;

			velocityY = final_vy;
			rb_other->velocityY = final_vy;
		}
	}
	else { // Dynamic vs Static
		if (collision_axis == 1) {
			velocityX = 0.f;
		}
		else if (collision_axis == 2) {
			velocityY = 0.f;

			// another way of ground detection
			/*glm::vec3 owner_center = (owner->AABB->min_point_world + owner->AABB->max_point_world) / 2.0f;
			glm::vec3 other_center = (other->AABB->min_point_world + other->AABB->max_point_world) / 2.0f;

			if (owner_center.y > other_center.y) {
				if (velocityY < 0) {
					isGrounded = true;
				}
			}*/
		}
	}
}

void Rigidbody::onExitCollision(Node* other)
{
}



