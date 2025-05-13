#pragma once

#include "Script.h"

class PlayerController : public Script
{
public:

    using SelfType = PlayerController;

    VARIABLE(float, speed);
    VARIABLE(float, jumpForce);
    VARIABLE(Node*, doors);
    VARIABLE(int, id);
    
	float velocityX = 0.0f;
    float velocityY = 0.0f;
    float gravity = -20.f;
	bool isGrounded = false;
    bool isJumping = false;


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

	void Jump();

    std::vector<Variable*> getFields() const override { 
        static Variable speedVar = getField_speed();
        static Variable jumpForceVar = getField_jumpForce();
        static Variable doorsVar = getField_doors();
        static Variable idVar = getField_id();
        return { &speedVar, &jumpForceVar, &doorsVar, &idVar };
    }
};

