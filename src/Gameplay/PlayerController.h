#pragma once

#include "Script.h"
#include "../System/Rigidbody.h"

class InstanceManager;
class TextObject;
class SpriteObject;
class PlayerRewindable;

class PlayerController final : public Script
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
    bool isDying = false;
	bool isInGas = false;
    bool isRunning = false;
    bool canJump = true;
	bool isElectrified = false;

    TextObject* virusTypeText;
    TextObject* virusEffectText;
    SpriteObject* cheeseSprite;
	std::string lastVirusType = "null";

    //bool inGas = false;
    bool is_pressed = false;
	std::string virusType = "none";
    float deathTimer;
    float gasTimer;
    float gasCheckTimer;
    Node* timerIndicator;
    InstanceManager* emitter;
    bool face_right = true;
    float scale_factor = 1.f;
	bool pressedRight = false;
	bool pressedLeft = false;
    bool jumpPressed = false;

    bool debugMode = false;
    bool debugTogglePressed = false;

    int sfxId = -1;

	PlayerRewindable* rewindable = nullptr;

    PlayerController() = default;
    virtual ~PlayerController() = default;

    bool isPadButtonPressed(int button);

    float getPadAxis(int axis);

    void onAttach(Node* owner) override;

    void onDetach() override;

    void onStart() override;

    void onUpdate(float deltaTime) override;

    void onEnd() override;

    void Die(bool freeze, bool electrified = false);

    bool HandleVirus(float deltaTime);

    void ApplyVirusEffect();

    void VirusEffect();

    bool CheckIfInGas();
    
    std::vector<Variable*> getFields() const override { 
        static Variable speedVar = getField_speed();
        static Variable jumpForceVar = getField_jumpForce();
        static Variable doorsVar = getField_doors();
        static Variable idVar = getField_id();
		static Variable isGravityFlippedVar = getField_isGravityFlipped();

        return { &speedVar, &jumpForceVar, &doorsVar, &idVar, &isGravityFlippedVar };
    }
};

