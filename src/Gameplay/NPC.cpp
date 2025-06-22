#include "NPC.h"
#include "RegisterScript.h"
#include "../System/PhysicsSystem.h"
#include "../Component/BoundingBox.h"
#include "../Basic/Node.h"
#include <algorithm>
#include <cmath>
#include "GameManager.h"
#include "../System/Tag.h"

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

    owner->AABB->addIgnoredLayer(TagLayerManager::Instance().getLayer("Platform"));
    owner->AABB_logic->addIgnoredLayer(TagLayerManager::Instance().getLayer("Platform"));

    startPos = owner->transform.getGlobalPosition();
    rb = owner->getComponent<Rigidbody>();
	rb->smoothingFactor = .02f;
	rb->drag = 1.5f;
    obstacleLayer = { "Wall", "Floor", "Door" };

    agentColliderSize = (owner->AABB->max_point_world.y - owner->AABB->min_point_world.y) / 2.f;

	spinnerLeft = owner->getChildByNamePart("wing_left");
	spinnerRight = owner->getChildByNamePart("wing_right");

	glm::vec3 pos = owner->transform.getGlobalPosition();
    auto* audio = ServiceLocator::getAudioEngine();
    sfxId = audio->PlayMusic(audio->propeller, /*GameManager::instance().sfxVolume **/ 75.f, pos);
	audio->SetChannel3dMinMaxDistance(sfxId, 1.0f, 16.0f);
}

void NPC::onUpdate(float deltaTime) {
    if (spinnerLeft && spinnerRight) {
        glm::quat currentRotation = spinnerLeft->transform.getLocalRotation();
        float rotationAnlge = 25.f * deltaTime;
        currentRotation = glm::rotate(currentRotation, rotationAnlge, glm::vec3(0.f, 0.f, 1.f));
        spinnerLeft->transform.setLocalRotation(currentRotation);
        spinnerRight->transform.setLocalRotation(-currentRotation);
    }

    /*if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_R) == GLFW_PRESS) {
        isActive = true;
        owner->setPhysic(true);
        owner->getComponent<Rigidbody>()->is_static = false;
        isCatched = false;
    }*/

    auto* audio = ServiceLocator::getAudioEngine();
    audio->SetChannel3dPosition(sfxId, owner->transform.getGlobalPosition());

    if (!isActive) return;

	ownerPos = (owner->AABB->max_point_world + owner->AABB->min_point_world) / 2.f;

    danger.fill(0.f);
    interest.fill(0.f);

    detectObstacles();
    detectPlayer();
    avoidObstacles();
    getSteering(deltaTime);

    glm::vec2 direction = getMoveDirection();
    move(direction, deltaTime);
}

void NPC::onCollision(Node* other)
{
    if (other->getTagName() == "Player") {
        isActive = false;
        owner->setPhysic(false);
        owner->getComponent<Rigidbody>()->is_static = true;
        isCatched = true;
    }
}

void NPC::detectObstacles() {
    obstacles.clear();

    /*for (Collider* box : PhysicsSystem::instance().getColliders()) {
        if (box->node) {
            Node* node = box->node;
            if (std::find(obstacleLayer.begin(), obstacleLayer.end(), node->getLayerName()) != obstacleLayer.end()) {
                glm::vec3 pos = node->transform.getGlobalPosition();
                glm::vec3 scale = node->transform.getLocalScale();

                float distance = glm::distance(glm::vec2(ownerPos), glm::vec2(pos));

                if (distance <= detectionRadius) {
                    obstacles.push_back(node);
                }
            }
        }
    }*/

    for (Collider* box : PhysicsSystem::instance().getColliders()) {
        if (box->node) {
            Node* node = box->node;

            std::string layerName = node->getLayerName();

            if (layerName == "Platform") continue;

            if (std::find(obstacleLayer.begin(), obstacleLayer.end(), layerName) != obstacleLayer.end()) {
                glm::vec3 pos = node->transform.getGlobalPosition();
                glm::vec3 scale = node->transform.getLocalScale();

                float distance = glm::distance(glm::vec2(ownerPos), glm::vec2(pos));
                if (distance <= detectionRadius) {
                    obstacles.push_back(node);
                }
            }
        }
    }
}

void NPC::detectPlayer() {
    for (Collider* box : PhysicsSystem::instance().getColliders()) {
        if (box->node && box->node->getTagName() == "Player") {
            playerPos = (box->node->AABB->max_point_world + box->node->AABB->min_point_world) / 2.f;

			float distance = glm::distance(ownerPos, playerPos);
            
            if (distance <= playerDetectionRadius) {
                // Sprawdzenie linii widocznoœci
                glm::vec3 direction = playerPos - ownerPos;
                Ray ray{ ownerPos, glm::normalize(direction) };
                std::vector<RayCastHit> hitNodes;
                if (PhysicsSystem::instance().rayCast(ray, hitNodes, distance, owner)) {
                    if (hitNodes.size() > 0 && hitNodes[0].node->getTagName() == "Player") {
                        
                        for (auto hit : hitNodes) {
                            std::cout << hit.node->getTagName() << std::endl;
                        }
                        
                        playerVisible = true;
                        return;
                    }
                    else if (hitNodes.size() > 1 && hitNodes[1].node->getTagName() == "Player" && hitNodes[0].node->getLayerName() == "Platform") {
                        playerVisible = true;
                        return;
                    }
                }
            }
        }
    }
    playerVisible = false;
}

void NPC::avoidObstacles() {
    for (Node* obstacle : obstacles) {
        glm::vec3 obstaclePos = obstacle->transform.getGlobalPosition();
        glm::vec3 obstacleHalfExtents = obstacle->transform.getLocalScale() * 0.5f;

        glm::vec3 closestPoint = getClosestPointOnAABB(ownerPos, obstaclePos, obstacleHalfExtents);
        glm::vec2 directionToObstacle = glm::vec2(closestPoint) - glm::vec2(ownerPos);

        float distanceToObstacle = glm::length(directionToObstacle);

        if (distanceToObstacle == 0.f) continue;

        float normalizedDistance = glm::clamp(distanceToObstacle / avoidanceRadius, 0.f, 1.f);
        float weight = powf(1.f - normalizedDistance, 3.f);

        //float weight = distanceToObstacle <= agentColliderSize ? 1.f : (avoidanceRadius - distanceToObstacle) / avoidanceRadius;
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
        playerPositionCached = glm::vec2(playerPos);
    }

    if (playerVisible || isFleeingWithoutTarget) {
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

            // --- NOWA, M¥DRZEJSZA LOGIKA ---
            std::vector<int> safeDirections;
            for (size_t i = 0; i < danger.size(); ++i) {
                // Wybierz kierunki, w których zagro¿enie jest minimalne
                if (danger[i] < 0.1f) {
                    safeDirections.push_back(i);
                }
            }

            if (!safeDirections.empty()) {
                // Losuj kierunek tylko spoœród bezpiecznych opcji
                int randomIndex = rand() % safeDirections.size();
                currentWanderDirectionIndex[0] = safeDirections[randomIndex];
                // Mo¿esz wylosowaæ drugi kierunek dla bardziej zró¿nicowanego ruchu
                currentWanderDirectionIndex[1] = -1; // lub powtórzyæ logikê
            }
            else {
                // Jeœli nie ma bezpiecznych kierunków (bardzo ma³o prawdopodobne),
                // wybierz kierunek o najmniejszym zagro¿eniu lub ka¿ NPC siê obróciæ.
                // Na przyk³ad, znajdŸ kierunek z najmniejsz¹ wartoœci¹ w 'danger'.
                auto minIt = std::min_element(danger.begin(), danger.end());
                currentWanderDirectionIndex[0] = std::distance(danger.begin(), minIt);
                currentWanderDirectionIndex[1] = -1;
            }
        }
    }
}

glm::vec2 NPC::getMoveDirection() {
    // 1. Oblicz wektor celu (na podstawie 'interest')
    glm::vec2 goalVector(0.f);
    float totalInterest = 0.f;
    for (size_t i = 0; i < interest.size(); ++i) {
        if (danger[i] < 0.1f) interest[i] += 0.5f;
        if (danger[i] < 0.25f && interest[i] < 0.1f) interest[i] += 0.5f;

        goalVector += eightDirections[i] * interest[i];
        totalInterest += interest[i];
    }
    if (totalInterest > 0) {
        goalVector /= totalInterest;
    }

    glm::vec2 avoidanceVector(0.f);
    for (size_t i = 0; i < danger.size(); ++i) {
        avoidanceVector -= eightDirections[i] * danger[i];
    }

    float avoidanceWeight = 1.5f;
    float goalWeight = 1.5f;

    glm::vec2 finalDirection = goalVector * goalWeight + avoidanceVector * avoidanceWeight;

    return finalDirection == glm::vec2(0.f) ? glm::vec2(0.f) : glm::normalize(finalDirection);
}

void NPC::move(const glm::vec2& direction, float deltaTime) {
    if (!rb) return;

    float currentSpeed = maxSpeed;
    if (isWandering) {
        currentSpeed *= 0.4f;
    }

    rb->targetVelocityX = direction.x * currentSpeed;
    rb->targetVelocityY = direction.y * currentSpeed;

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
