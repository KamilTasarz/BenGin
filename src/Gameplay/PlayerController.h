#pragma once

#include "Script.h"



class PlayerController : public Script
{
public:

    using SelfType = PlayerController;

    VARIABLE(float, speed);
    VARIABLE(Node*, doors);
    VARIABLE(Node*, mouse);
    VARIABLE(int, id);
    VARIABLE(bool, gravity);
    

    PlayerController() = default;
    virtual ~PlayerController() = default;

    void onAttach(Node* owner) override;

    void onDetach() override;

    void onStart() override;

    void onUpdate(float deltaTime) override;

    void onEnd() override;

    void onCollision(Node* other) override;
    void onStayCollision(Node* other) override;
    void onExitCollision(Node* other) override;

    void onCollisionLogic(Node* other) override;
    void onStayCollisionLogic(Node* other) override;
    void onExitCollisionLogic(Node* other) override;

    std::vector<Variable*> getFields() const override { 
        static Variable speedVar = getField_speed();
        static Variable doorsVar = getField_doors();
		static Variable mouseVar = getField_mouse();
        static Variable idVar = getField_id();
		static Variable gravityVar = getField_gravity();
        return { &speedVar, &doorsVar, &idVar, &mouseVar, &gravityVar };
    }
};

