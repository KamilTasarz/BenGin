#pragma once

#include "Script.h"
#include "../System/Rigidbody.h"

class PlayerController : public Script
{
public:

    using SelfType = PlayerController;

    VARIABLE(float, speed);
    VARIABLE(float, jumpForce);
    VARIABLE(Node*, doors);
    VARIABLE(Node*, mouse);
    VARIABLE(int, id);
	VARIABLE(bool, isGravityFlipped);
    
	Rigidbody* rb = nullptr;
	bool isGrounded = false;
    bool isJumping = false;
    bool isDead = false;
	std::string virusType = "none";
    float deathTimer;
    Node* timerIndicator;

    PlayerController() = default;
    virtual ~PlayerController() = default;

    void onAttach(Node* owner) override;

    void onDetach() override;

    void onStart() override;

    void onUpdate(float deltaTime) override;

    void onEnd() override;

    void Die(bool freeze, bool electrified = false);

    void HandleVirus(float deltaTime);

    std::vector<Variable*> getFields() const override { 
        static Variable speedVar = getField_speed();
        static Variable jumpForceVar = getField_jumpForce();
        static Variable doorsVar = getField_doors();
        static Variable idVar = getField_id();
		static Variable isGravityFlippedVar = getField_isGravityFlipped();

        return { &speedVar, &jumpForceVar, &doorsVar, &idVar, &isGravityFlippedVar };
    }
};

