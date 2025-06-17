#pragma once

#include "Script.h"
#include <vector>
#include <array>
#include <memory>
#include "../System/Rigidbody.h"

class NPC : public Script {
public:
    using SelfType = NPC;

    // Parametry konfigurowalne w edytorze
    VARIABLE(bool, isActive);
    VARIABLE(bool, isFleeing);
    VARIABLE(float, detectionRadius);
    VARIABLE(float, playerDetectionRadius);
    VARIABLE(float, avoidanceRadius);
    VARIABLE(float, agentColliderSize);
    VARIABLE(float, fleeAngle);
    VARIABLE(float, continueFleeTime);
    VARIABLE(float, minMoveTime);
    VARIABLE(float, maxMoveTime);
    VARIABLE(float, minWaitTime);
    VARIABLE(float, maxWaitTime);
    VARIABLE(float, maxSpeed);

    bool isCatched = false;
    glm::vec3 startPos;
	int sfxId = -1;

    glm::vec2 smoothedDirection{ 0.f, 0.f };
    float directionSmoothing = 7.5f;

	Node* spinnerLeft = nullptr;
	Node* spinnerRight = nullptr;

    NPC() = default;
    ~NPC() = default;

    void onAttach(Node* owner) override;
    void onDetach() override;
    void onStart() override;
    void onUpdate(float deltaTime) override;
    void onCollision(Node* other) override;

    std::vector<Variable*> getFields() const override {
		static Variable isActiveVar = getField_isActive();
		static Variable isFleeingVar = getField_isFleeing();
		static Variable detectionRadiusVar = getField_detectionRadius();
		static Variable playerDetectionRadiusVar = getField_playerDetectionRadius();
		static Variable avoidanceRadiusVar = getField_avoidanceRadius();
		static Variable agentColliderSizeVar = getField_agentColliderSize();
		static Variable fleeAngleVar = getField_fleeAngle();
		static Variable continueFleeTimeVar = getField_continueFleeTime();
		static Variable minMoveTimeVar = getField_minMoveTime();
		static Variable maxMoveTimeVar = getField_maxMoveTime();
		static Variable minWaitTimeVar = getField_minWaitTime();
		static Variable maxWaitTimeVar = getField_maxWaitTime();
		static Variable maxSpeedVar = getField_maxSpeed();

		return { &isActiveVar, &isFleeingVar, &detectionRadiusVar, &playerDetectionRadiusVar, &avoidanceRadiusVar,
				 &agentColliderSizeVar, &fleeAngleVar, &continueFleeTimeVar, &minMoveTimeVar,
				 &maxMoveTimeVar, &minWaitTimeVar, &maxWaitTimeVar, &maxSpeedVar };
    }

public:
    enum class WanderState { Waiting, Moving };

    Rigidbody* rb;
    bool playerVisible = false;
    std::vector<Node*> obstacles;
    std::vector<std::string> obstacleLayer;

    std::array<float, 8> danger{};
    std::array<float, 8> interest{};

    glm::vec3 ownerPos;
    glm::vec3 playerPos;

    bool isFleeingWithoutTarget = false;
    bool isWandering = true;
    bool escaped = true;

    float fleeTimer = 0.f;
    glm::vec2 playerPositionCached{ 0.f, 0.f };

    WanderState wanderState = WanderState::Waiting;
    float wanderTimer = 0.f;
    float currentWanderDuration = 0.f;
    float currentWaitDuration = 0.f;
    std::array<int, 2> currentWanderDirectionIndex{ {-1, -1} };

    float currentSpeed = 0.f;
    glm::vec2 smoothedMovementInput{ 0.f, 0.f };

    float updateTimer = 0.2f;

    void detectObstacles();
    void detectPlayer();
    void avoidObstacles();
    void getSteering(float deltaTime);
    glm::vec2 getMoveDirection();
    void move(const glm::vec2& direction, float deltaTime);
    glm::vec3 getClosestPointOnAABB(const glm::vec3& point, const glm::vec3& boxCenter, const glm::vec3& boxHalfExtents);
};
