#include "NPC.h"
#include "RegisterScript.h"
#include "../System/PhysicsSystem.h"
#include "../Component/BoundingBox.h"
#include "../Basic/Node.h"
#include <algorithm>
#include <cmath>

REGISTER_SCRIPT(NPC);

// Osiem kierunków ruchu
static const std::array<glm::vec2, 8> eightDirections = {
    glm::normalize(glm::vec2(0, 1)),
    glm::normalize(glm::vec2(1, 1)),
    glm::normalize(glm::vec2(1, 0)),
    glm::normalize(glm::vec2(1, -1)),
    glm::normalize(glm::vec2(0, -1)),
    glm::normalize(glm::vec2(-1, -1)),
    glm::normalize(glm::vec2(-1, 0)),
    glm::normalize(glm::vec2(-1, 1))
};

void NPC::onAttach(Node* owner) {
    this->owner = owner;
}

void NPC::onDetach() {
    owner = nullptr;
}

void NPC::onStart() {
    // Inicjalizacja, np. znalezienie gracza w scenie
    // player = owner->scene_graph->findNodeByTag("Player");

    rb = owner->getComponent<Rigidbody>();
	rb->smoothingFactor = 5.f;
    obstacleLayer = { "Wall", "Floor" };
}

void NPC::onUpdate(float deltaTime) {
    danger.fill(0.f);
    interest.fill(0.f);

    detectObstacles();
    detectPlayer();
    avoidObstacles();
    getSteering(deltaTime);

    glm::vec2 direction = getMoveDirection();
    move(direction, deltaTime);
}

void NPC::detectObstacles() {
    glm::vec3 ownerPos = owner->transform.getGlobalPosition();

    obstacles.clear();
    for (BoundingBox* box : PhysicsSystem::instance().getColliders()) {
        if (box->node) {
            Node* node = box->node;
            if (std::find(obstacleLayer.begin(), obstacleLayer.end(), node->getLayerName()) != obstacleLayer.end()) {
                glm::vec3 pos = node->transform.getGlobalPosition();
                glm::vec3 scale = node->transform.getLocalScale();

                float distance = glm::distance(glm::vec2(ownerPos), glm::vec2(pos));

                if (distance <= detectionRadius) {
                    // Opcjonalnie mo¿esz zostawiæ sprawdzanie "czy w œrodku" jako drugi warunek
                    /*float halfW = scale.x / 2.f;
                    float halfH = scale.y / 2.f;

                    if (ownerPos.x >= pos.x - halfW && ownerPos.x <= pos.x + halfW &&
                        ownerPos.y >= pos.y - halfH && ownerPos.y <= pos.y + halfH) {
                        obstacles.push_back(node);
                    }*/

                    obstacles.push_back(node);
                }
            }
        }
    }
}

void NPC::detectPlayer() {
    // Zak³adamy, ¿e gracz ma tag "Player"
    for (BoundingBox* box : PhysicsSystem::instance().getColliders()) {
        if (box->node && box->node->getTagName() == "Player") {
			glm::vec3 playerPos = box->node->transform.getGlobalPosition();
			glm::vec3 ownerPos = owner->transform.getGlobalPosition();
            
			float distance = glm::distance(ownerPos, playerPos);
            
            if (distance <= playerDetectionRadius) {
                // Sprawdzenie linii widocznoœci
                glm::vec3 direction = playerPos - ownerPos;
                Ray ray{ ownerPos, glm::normalize(glm::vec4(direction, 0.f)) };
                std::vector<Node*> hitNodes;
                if (PhysicsSystem::instance().rayCast(ray, hitNodes, distance, owner)) {
                    if (hitNodes.size() > 0 && hitNodes[0]->getTagName() == "Player") {
                        player = hitNodes[0];
                        return;
                    }
                }
            }
        }
    }
    player = nullptr;
}

void NPC::avoidObstacles() {
    for (Node* obstacle : obstacles) {
        glm::vec3 obstaclePos = obstacle->transform.getGlobalPosition();
        glm::vec3 obstacleHalfExtents = obstacle->transform.getLocalScale() * 0.5f;
        glm::vec3 ownerPos = owner->transform.getGlobalPosition();

        glm::vec3 closestPoint = getClosestPointOnAABB(ownerPos, obstaclePos, obstacleHalfExtents);
        glm::vec2 directionToObstacle = glm::vec2(closestPoint) - glm::vec2(ownerPos);
        float distanceToObstacle = glm::length(directionToObstacle);
        if (distanceToObstacle == 0.f) continue;

        float weight = distanceToObstacle <= agentColliderSize ? 1.f : (avoidanceRadius - distanceToObstacle) / avoidanceRadius;
        glm::vec2 directionToObstacleNormalized = glm::normalize(directionToObstacle);

        for (size_t i = 0; i < eightDirections.size(); ++i) {
            float result = glm::dot(directionToObstacleNormalized, eightDirections[i]);
            if (result > 0.f) {
                float valueToPutIn = result * weight;
                if (valueToPutIn > danger[i]) {
                    danger[i] = valueToPutIn;
                }
            }
        }
    }
}

void NPC::getSteering(float deltaTime) {
    bool playerVisible = player != nullptr;

    if (!playerVisible) {
        if (!isFleeingWithoutTarget && !escaped) {
            isFleeingWithoutTarget = true;
            fleeTimer = continueFleeTime;
        }
        else if (!escaped) {
            fleeTimer -= deltaTime;
            if (fleeTimer <= 0.f) {
                isFleeingWithoutTarget = false;
                escaped = true;
                isWandering = true;
            }
        }
    }
    else {
        isFleeingWithoutTarget = false;
        escaped = false;
        isWandering = false;
		glm::vec3 playerPos = player->transform.getGlobalPosition();
        playerPositionCached = glm::vec2(playerPos);
    }

    if (playerVisible || isFleeingWithoutTarget) {
		glm::vec3 ownerPos = owner->transform.getGlobalPosition();

        glm::vec2 directionToTarget = playerPositionCached - glm::vec2(ownerPos);
        glm::vec2 fleeDirection = isFleeing ? -glm::normalize(directionToTarget) : glm::normalize(directionToTarget);

        if (glm::distance(glm::vec2(ownerPos), playerPositionCached) < 1.2f) fleeTimer = -1.f;

        float angleThreshold = std::cos(glm::radians(fleeAngle));

        for (size_t i = 0; i < interest.size(); ++i) {
            float result = glm::dot(fleeDirection, eightDirections[i]);
            if (result > angleThreshold) {
                float valueToPutIn = (result - angleThreshold) / (1.f - angleThreshold);
                if (valueToPutIn > interest[i]) {
                    interest[i] = valueToPutIn;
                }
            }
        }

        if (playerVisible) {
			std::cout << "NPC " << owner->getName() << " widzi gracza: " << player->getName() << std::endl;
        }
        else if (isFleeingWithoutTarget) {
            std::cout << "NPC " << owner->getName() << " ucieka przed graczem " << std::endl;
        }
		else if (escaped) {
			std::cout << "NPC " << owner->getName() << " uciek³ przed graczem " << std::endl;
		}
		else if (isWandering) {
			std::cout << "NPC " << owner->getName() << " wêdruje" << std::endl;
		}
     
        return;
    }

    // Wandering
    wanderTimer -= deltaTime;

    if (wanderState == WanderState::Moving) {
        if (wanderTimer <= 0.f) {
            wanderState = WanderState::Waiting;
            currentWaitDuration = minWaitTime + static_cast<float>(rand()) / RAND_MAX * (maxWaitTime - minWaitTime);
            wanderTimer = currentWaitDuration;
        }
        else {
            if (currentWanderDirectionIndex[0] >= 0 && currentWanderDirectionIndex[0] < static_cast<int>(interest.size()))
                interest[currentWanderDirectionIndex[0]] = 0.2f;

            if (currentWanderDirectionIndex[1] >= 0 && currentWanderDirectionIndex[1] < static_cast<int>(interest.size()))
                interest[currentWanderDirectionIndex[1]] = 0.2f;
        }
    }
    else if (wanderState == WanderState::Waiting) {
        if (wanderTimer <= 0.f) {
            wanderState = WanderState::Moving;
            currentWanderDuration = minMoveTime + static_cast<float>(rand()) / RAND_MAX * (maxMoveTime - minMoveTime);
            wanderTimer = currentWanderDuration;
            currentWanderDirectionIndex[0] = rand() % eightDirections.size();
            currentWanderDirectionIndex[1] = rand() % eightDirections.size();
        }
    }
}

glm::vec2 NPC::getMoveDirection() {
    for (size_t i = 0; i < interest.size(); ++i) {
        interest[i] = std::clamp(interest[i] - danger[i], 0.f, 1.f);

        if (danger[i] < 0.1f) interest[i] += 0.2f;
        if (danger[i] < 0.2f && interest[i] < 0.1f) interest[i] += 0.3f;
    }

    glm::vec2 output{ 0.f, 0.f };
    for (size_t i = 0; i < interest.size(); ++i)
        output += eightDirections[i] * interest[i];

	//std::cout << "NPC " << owner->getName() << " kierunek ruchu: " << output.x << ", " << output.y << std::endl;

    return output == glm::vec2(0.f) ? glm::vec2(0.f) : glm::normalize(output);
}

void NPC::move(const glm::vec2& direction, float deltaTime) {
    bool hasDirection = direction.x != 0.f || direction.y != 0.f;

	if (!hasDirection) {
		return;
	}

    if (!rb) return;

    float velocityX = glm::clamp(direction.x, -maxSpeed, maxSpeed);
    float velocityY = glm::clamp(direction.y, -maxSpeed, maxSpeed);

    float speed = maxSpeed;
    if (isWandering) {
        speed *= 0.4f;
    }

    // Ustaw celow¹ prêdkoœæ na osi X
    rb->targetVelocityX = velocityX * speed;

    // Ustaw celow¹ prêdkoœæ na osi Y (jeœli NPC mo¿e siê poruszaæ pionowo)
    rb->targetVelocityY = velocityY * speed;

    // Informujemy Rigidbody, ¿e ma nadpisaæ domyœlne velocity
    rb->overrideVelocityX = true;
    rb->overrideVelocityY = true;
}

glm::vec3 NPC::getClosestPointOnAABB(const glm::vec3& point, const glm::vec3& boxCenter, const glm::vec3& boxHalfExtents) {
    glm::vec3 closest;
    closest.x = glm::clamp(point.x, boxCenter.x - boxHalfExtents.x, boxCenter.x + boxHalfExtents.x);
    closest.y = glm::clamp(point.y, boxCenter.y - boxHalfExtents.y, boxCenter.y + boxHalfExtents.y);
    closest.z = glm::clamp(point.z, boxCenter.z - boxHalfExtents.z, boxCenter.z + boxHalfExtents.z);
    return closest;
}
