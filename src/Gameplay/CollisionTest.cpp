#include "CollisionTest.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"

REGISTER_SCRIPT(CollisionTest);

void CollisionTest::onAttach(Node* owner)
{
	this->owner = owner;
	std::cout << "CollisionTest::onAttach::" << owner->name << std::endl;
}

void CollisionTest::onDetach()
{
	std::cout << "CollisionTest::onDetach::" << owner->name << std::endl;
	owner = nullptr;
}

void CollisionTest::onUpdate(float deltaTime)
{
	//std::cout << "CollisionTest::onUpdate::" << owner->name << std::endl;
}

void CollisionTest::onCollision(Node* other)
{
	std::cout << "CollisionTest::onCollision::" << owner->name << std::endl;
}

void CollisionTest::onStayCollision(Node* other)
{
	std::cout << "CollisionTest::onStayCollision::" << owner->name << std::endl;
}

void CollisionTest::onExitCollision(Node* other)
{
	std::cout << "CollisionTest::onExitCollision::" << owner->name << std::endl;
}

void CollisionTest::onCollisionLogic(Node* other)
{
	std::cout << "CollisionTest::onCollisionLogic::" << owner->name << std::endl;
}

void CollisionTest::onStayCollisionLogic(Node* other)
{
	std::cout << "CollisionTest::onStayCollisionLogic::" << owner->name << std::endl;
}

void CollisionTest::onExitCollisionLogic(Node* other)
{
	std::cout << "CollisionTest::onExitCollisionLogic::" << owner->name << std::endl;
}
