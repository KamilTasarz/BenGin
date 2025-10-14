#pragma once

#include "Script.h"
#include <unordered_set>
#include <queue>
#include "GameManager.h"

class InstanceManager;

class GasWall : public Script {
public:
    using SelfType = GasWall;

    VARIABLE(float, spreadSpeed);
    VARIABLE(bool, spreading);

    std::shared_ptr<Prefab> prefab;

    float spreadInterval = 1.f;
    float timer = 0.f;

    std::vector<std::string> obstacleLayer;
    std::queue<glm::ivec2> spreadQueue;
    std::unordered_set<std::string> visited;
    //std::deque<std::string> visitedQueue;
    std::unordered_set<std::string> blocked;
    std::unordered_set<std::string> freeButUnused;

    int counter = 0;
	bool inputDetected = false;

    InstanceManager* gasCreator;

    GasWall() = default;
    ~GasWall() = default;

    void onAttach(Node* owner) override;
    void onDetach() override;
    void onStart() override;
    void onUpdate(float deltaTime) override;
    void onCollisionLogic(Node* other) override;

    void spreadCloud();
    std::string posKey(const glm::ivec2& pos) const;

    std::vector<Variable*> getFields() const override {
        static Variable spreadSpeedVar = getField_spreadSpeed();
        static Variable spreadingVar = getField_spreading();
        return { &spreadSpeedVar, &spreadingVar };
    }
};
