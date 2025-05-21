#pragma once

#include "Script.h"
#include <queue>

class GasWall : public Script
{
public:
	using SelfType = GasWall;

	VARIABLE(float, spreadSpeed);

	float spreadInterval = 1.f;
	float timer;
	bool spreading = true;
	std::vector<std::string> obstacleLayer;
	std::queue<glm::vec3> spreadQueue;
	std::unordered_set<std::string> visited;
	std::shared_ptr<Prefab> prefab;
	int counter;

	GasWall() = default;	
	~GasWall() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void spreadCloud(float deltaTime);
	std::string posKey(const glm::vec3& pos);
	//void onStayCollisionLogic(Node* other) override;

	std::vector<Variable*> getFields() const override {
		static Variable spreadSpeedVar = getField_spreadSpeed();
		return { &spreadSpeedVar };
	}
};

