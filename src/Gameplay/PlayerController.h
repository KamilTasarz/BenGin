#pragma once

#include "Script.h"

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
    
	bool isGrounded = false;
    bool isJumping = false;
	std::string virusType = "none";


    PlayerController() = default;
    virtual ~PlayerController() = default;

    void onAttach(Node* owner) override;

    void onDetach() override;

    void onStart() override;

    void onUpdate(float deltaTime) override;

    void onEnd() override;

    std::vector<Variable*> getFields() const override { 
        static Variable speedVar = getField_speed();
        static Variable jumpForceVar = getField_jumpForce();
        static Variable doorsVar = getField_doors();
        static Variable idVar = getField_id();
		static Variable isGravityFlippedVar = getField_isGravityFlipped();

        return { &speedVar, &jumpForceVar, &doorsVar, &idVar, &isGravityFlippedVar };
    }
};

