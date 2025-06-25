#include "PlayerController.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "../System/Tag.h"
#include "PlayerSpawner.h"
#include "GroundObject.h"
#include "../Basic/Animator.h"
#include "../System/PhysicsSystem.h"
#include "../Component/BoundingBox.h"
#include "Animation/PlayerAnimationController.h"
#include "GameMath.h"
#include "GameManager.h"
#include "../System/GuiManager.h"
#include "../AudioEngine.h"
#include "PlayerRewindable.h"
#include "../Component/CameraGlobals.h"
#include "CameraFollow.h"
#include "../ResourceManager.h"
#include "Animation/DeathState.h"


//#include "GameMath.h"

enum class UIAnimState {
	Hidden,
	Entering,
	Visible,
	Exiting
};

struct UIAnimator {
	UIAnimState state = UIAnimState::Hidden;
	float timer = 0.f;
	float duration = .2f;
	glm::vec2 hiddenPos;
	glm::vec2 overshootPos;
	glm::vec2 targetPos;

	UIAnimator(glm::vec2 target)
		: targetPos(target) {
		overshootPos = target - glm::vec2(50.f, 0.f);
		hiddenPos = target + glm::vec2(50.f, 0.f);
	}

	void Show() {
		state = UIAnimState::Entering;
		timer = 0.f;
	}

	void Hide() {
		state = UIAnimState::Exiting;
		timer = 0.f;
	}

	void Update(float deltaTime, GuiObject* element) {
		timer += deltaTime;

		float t = glm::clamp(timer / duration, 0.f, 1.f);

		if (state == UIAnimState::Entering) {
			if (t < 0.5f) {
				float subT = t / 0.5f;
				element->pos = glm::mix(hiddenPos, overshootPos, subT);
			}
			else {
				float subT = (t - 0.5f) / 0.5f;
				element->pos = glm::mix(overshootPos, targetPos, subT);
			}

			if (t >= 1.f) {
				state = UIAnimState::Visible;
			}
		}
		else if (state == UIAnimState::Exiting) {
			if (t < 0.5f) {
				float subT = t / 0.5f;
				element->pos = glm::mix(targetPos, overshootPos, subT);
			}
			else {
				float subT = (t - 0.5f) / 0.5f;
				element->pos = glm::mix(overshootPos, hiddenPos, subT);
			}

			if (t >= 1.f) {
				element->pos = glm::vec2(2000, 2000);
				state = UIAnimState::Hidden;
				element->visible = false;
			}
		}
	}
};

UIAnimator virusTypeAnimator(glm::vec2(1665, 135));
UIAnimator virusEffectAnimator(glm::vec2(1665, 200));
UIAnimator cheeseSpriteAnimator(glm::vec2(1705, 145));

REGISTER_SCRIPT(PlayerController);

bool PlayerController::isPadButtonPressed(int button) {
	
	if (!glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) return false;
	GLFWgamepadstate state;
	if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
		return state.buttons[button] == GLFW_PRESS;
	}
	return false;

}

float PlayerController::getPadAxis(int axis) {

	if (!glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) return 0.0f;
	GLFWgamepadstate state;
	if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
		return state.axes[axis];
	}
	return 0.0f;
	
}

void PlayerController::onAttach(Node* owner) {
	this->owner = owner;
}

void PlayerController::onDetach() {
	owner = nullptr;
}

void PlayerController::onStart()
{
	owner->textures.clear();
	owner->textures.push_back(34);

	isGravityFlipped = false;
	deathTimer = 0.5f;
	rb = owner->getComponent<Rigidbody>();
	rb->lockPositionZ = true;
	rb->isPlayer = true;
	rb->dragY = 1.f;

	timerIndicator = owner->getChildByNamePart("timer");
	scale_factor = owner->transform.getLocalScale().x;
	emitter = dynamic_cast<InstanceManager*>(owner->scene_graph->root->getChildByTag("Emitter"));
	virusTypeText = GuiManager::Instance().findText(6);
	virusEffectText = GuiManager::Instance().findText(7);
	cheeseSprite = GuiManager::Instance().findSprite(8);

	rewindable = owner->getComponent<PlayerRewindable>();
	if (rewindable == nullptr) {
		owner->addComponent(std::make_unique<PlayerRewindable>());
		rewindable = owner->getComponent<PlayerRewindable>();
	}

	const auto ratTexture = ResourceManager::Instance().getTexture(34);
	owner->pModel->meshes[0].textures[0] = (ratTexture);
}

void PlayerController::onUpdate(float deltaTime)
{
	isGamepadConnected = glfwJoystickIsGamepad(GLFW_JOYSTICK_1);
	GameManager::instance().isGamepadConnected = isGamepadConnected;

	cout << "\nGamepad connected?: " << isGamepadConnected << endl;

	if (isDead) return;

	isDying = false;

	glm::vec3 position = owner->transform.getLocalPosition();
	glm::vec3 globalPosition = owner->transform.getGlobalPosition();
	globalPosition.z -= 2.f;

	auto* audio = ServiceLocator::getAudioEngine();
	audio->Set3dListenerAndOrientation(globalPosition, glm::vec3(rb->velocityX, rb->velocityY, 0.f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0, 1, 0));

	if (!rb) return;

	if (rb->groundUnderneath || rb->scaleUnderneath) {
		isJumping = false;
		canJump = true;
	}

	if (isGravityFlipped && !rb->isGravityFlipped) {
		rb->isGravityFlipped = true;
	}
	else if (!isGravityFlipped && rb->isGravityFlipped) {
		rb->isGravityFlipped = false;
	}

	if (!GameManager::instance().isRewinding) {

		const float axisX = getPadAxis(GLFW_GAMEPAD_AXIS_LEFT_X);
		const bool dpadLeft = isPadButtonPressed(GLFW_GAMEPAD_BUTTON_DPAD_LEFT);
		const bool dpadRight = isPadButtonPressed(GLFW_GAMEPAD_BUTTON_DPAD_RIGHT);

		pressedRight = (
			glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_RIGHT) == GLFW_PRESS || 
			glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_D) == GLFW_PRESS ||
			axisX > 0.1f ||
			dpadRight
			);
		
		pressedLeft = (
			glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_LEFT) == GLFW_PRESS || 
			glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_A) == GLFW_PRESS ||
			axisX < -0.1f ||
			dpadLeft
			);

		jumpPressed = (
			glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_SPACE) == GLFW_PRESS ||
			isPadButtonPressed(GLFW_GAMEPAD_BUTTON_A) ||
			isPadButtonPressed(GLFW_GAMEPAD_BUTTON_X)
			);

		if (!rb->overrideVelocityX) rb->targetVelocityX = (pressedRight - pressedLeft) * speed;

		if (pressedRight - pressedLeft != 0.f) isRunning = true;
		else isRunning = false;

		if (pressedRight) rb->side = glm::vec4(1.f, 0.f, 0.f, 0.f);
		else if (pressedLeft) rb->side = glm::vec4(-1.f, 0.f, 0.f, 0.f);

		// DebugMode
		if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS && !debugTogglePressed) {
			debugMode = !debugMode;
			debugTogglePressed = true;

			rb->is_static = debugMode;
			owner->setPhysic(!debugMode);

		}

		if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_GRAVE_ACCENT) == GLFW_RELEASE) {
			debugTogglePressed = false;
		}

		if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_9) == GLFW_PRESS) {
			int temp = ServiceLocator::getAudioEngine()->PlayMusic("res/audios/sounds/bonk.ogg");
		}

		if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_7) == GLFW_PRESS) {
			owner->scene_graph->activateRewindShader();
		}

		if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_8) == GLFW_PRESS) {
			owner->scene_graph->deactivateRewindShader();
		}

		if (debugMode) {
			glm::vec3 move(0.0f);

			if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_W) == GLFW_PRESS)
				move.y += 1.0f;
			if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_S) == GLFW_PRESS)
				move.y -= 1.0f;
			if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_D) == GLFW_PRESS)
				move.x += 1.0f;
			if (glfwGetKey(ServiceLocator::getWindow()->window, GLFW_KEY_A) == GLFW_PRESS)
				move.x -= 1.0f;

			owner->transform.setLocalPosition(owner->transform.getLocalPosition() + move * 50.f * deltaTime);

			return;
		}

		if (!jumpPressed && was_jump_pressed) {
			jump_released = true;
		}

		if (jumpPressed && !was_jump_pressed) {
			if (jump_released && (rb->groundUnderneath || rb->scaleUnderneath) && canJump) {
				if (isGravityFlipped)
					rb->velocityY = -jumpForce;
				else
					rb->velocityY = jumpForce;

				isJumping = true;
				canJump = false;

				jump_released = false;
			}
		}

		was_jump_pressed = jumpPressed;
	}

	if (virusType != "none") {
		isDying = HandleVirus(deltaTime);
	}
	else {
		timerIndicator->transform.setLocalScale(glm::vec3(0.f));
	}

	float checkDelay = 0.5f;
	if (gasCheckTimer > checkDelay) {

		isInGas = CheckIfInGas();

		if (isInGas) {
			gasTimer += checkDelay;

			if (gasTimer > 1.5f) {
				GameManager::instance().game_over = true;
			}
		}
		else {
			gasTimer = 0.f;
		}
		gasCheckTimer = 0.f;
	}
	else {
		gasCheckTimer += deltaTime;
	}

	// opis sera
	if (virusType == "none" && virusTypeAnimator.state == UIAnimState::Visible) {
		virusTypeAnimator.Hide();
		virusEffectAnimator.Hide();
		cheeseSpriteAnimator.Hide();

		ApplyVirusEffect();
	}
	else if (virusType != "none" && virusTypeAnimator.state == UIAnimState::Hidden || virusType != "none" && lastVirusType != virusType) {
		if (virusType == "blue") {
			virusTypeText->value = "Feather Feta";
			virusEffectText->value = "decreased weight";

			auto& spriteMap = GuiManager::Instance().getSprites();
			auto it = spriteMap.find(16);

			if (it != spriteMap.end()) {
				std::shared_ptr<Sprite> sprite = it->second;
				if (sprite) {
					// u¿yj sprite
					cheeseSprite->sprite = sprite;
				}
			}
		}
		else if (virusType == "green") {
			virusTypeText->value = "Gravity Gouda";
			virusEffectText->value = "inverted gravity";

			auto& spriteMap = GuiManager::Instance().getSprites();
			auto it = spriteMap.find(17);

			if (it != spriteMap.end()) {
				std::shared_ptr<Sprite> sprite = it->second;
				if (sprite) {
					// u¿yj sprite
					cheeseSprite->sprite = sprite;
				}
			}
		}
		else if (virusType == "black") {
			virusTypeText->value = "Heavy Havarti";
			virusEffectText->value = "increased weight";

			auto& spriteMap = GuiManager::Instance().getSprites();
			auto it = spriteMap.find(15);

			if (it != spriteMap.end()) {
				std::shared_ptr<Sprite> sprite = it->second;
				if (sprite) {
					// u¿yj sprite
					cheeseSprite->sprite = sprite;
				}
			}
		}

		virusEffectText->visible = true;
		virusTypeText->visible = true;
		cheeseSprite->visible = true;

		virusTypeAnimator.Show();
		virusEffectAnimator.Show();
		cheeseSpriteAnimator.Show();

		ApplyVirusEffect();
	}

	virusTypeAnimator.Update(deltaTime, virusTypeText);
	virusEffectAnimator.Update(deltaTime, virusEffectText);
	cheeseSpriteAnimator.Update(deltaTime, cheeseSprite);

	lastVirusType = virusType;

}

void PlayerController::onEnd() {}

void PlayerController::Die(bool freeze, bool electrified)
{
	if (isDead) return;

	timerIndicator->setActive(false);

	if (electrified) {
		rb->is_static = true;
		//isElectrified = true;
	}
	else if (freeze) {
		Rigidbody* rb = owner->getComponent<Rigidbody>();
		rb->is_static = true;
	}

	std::shared_ptr<Tag> tag = TagLayerManager::Instance().getTag("Box");
	owner->setTag(tag);

	std::shared_ptr<Layer> layer = TagLayerManager::Instance().getLayer("Body");
	owner->setLayer(layer);

	isDead = true;

	PlayerAnimationController *animationController = owner->getComponent<PlayerAnimationController>();
	animationController->changeState(new DeathState());

	auto* audio = ServiceLocator::getAudioEngine();
	audio->PlaySFX(audio->death, GameManager::instance().sfxVolume * 65.f);

	if (!GameManager::instance().tutorialActive) GameManager::instance().deathCount++;
}

bool PlayerController::HandleVirus(float deltaTime)
{
	timerIndicator->is_visible = true;
	constexpr float smoothing = 10.f;

	glm::vec3 currentScale = timerIndicator->transform.getLocalScale();
	glm::vec3 targetScale = glm::vec3(0.2f, deathTimer * 4.f, 0.2f);

	glm::vec3 newScale = glm::mix(currentScale, targetScale, deltaTime * smoothing);
	timerIndicator->transform.setLocalScale(newScale);

	//std::cout << "Gracz biegnie: " << isRunning << ", gracz skacze: " << isJumping << std::endl;

	if (isRunning || !rb->groundUnderneath || rewindable->isRewinding) {
		deathTimer = 0.8f;
		return false;
	}
	else {
		deathTimer -= deltaTime;

		if (deathTimer <= -0.1f) {
			Die(false);
		}

		if (deathTimer > 0.5f) {
			return false;
		}
		return true;
	}
}

void PlayerController::ApplyVirusEffect()
{

	rb->gravity = -32.f;
	rb->mass = 1.f;
	rb->dragY = 1.f;
	speed = 9.f;
	isGravityFlipped = false;
	jumpForce = 23.f;
	CameraFollow::instance->verticalOffset = 3.f;

	VirusEffect();
}

void PlayerController::VirusEffect()
{
	if (virusType == "blue") {
		owner->textures.clear();
		owner->textures.push_back(35);

		isGravityFlipped = false;
		rb->gravity = -32.f;
		rb->mass = 0.4f;
		rb->dragY = 0.5f;
		jumpForce *= 1.5f;
	}
	else if (virusType == "green") {

		owner->textures.clear();
		owner->textures.push_back(36);

		CameraFollow::instance->verticalOffset = -1.f;
		isGravityFlipped = true;
		rb->gravity = 32.f;
	}
	else if (virusType == "black") {

		owner->textures.clear();
		owner->textures.push_back(37);

		rb->mass = 25.f;
		speed *= 0.8f;
		jumpForce *= 0.85f;
	}
	else {
		//std::cout << "Unknown virus type!" << std::endl;
	}
}

bool PlayerController::CheckIfInGas() {
	bool inGas = false;

	float minDist = 1000.f, dist = 0.f;

	int index = emitter->tail - 1, iterator = 0;
	glm::vec3 playerPos = owner->transform.getGlobalPosition();
	isGravityFlipped ? playerPos.y -= 0.5f : playerPos.y += 0.5f;

	for (int i = 0; i < emitter->size; i++) {
		dist = glm::distance(glm::vec2(emitter->particles[index].position), glm::vec2(playerPos));
		if (dist < 0.8f) {

			inGas = true;
			break;

		}
		index--;
		if (index < 0) {
			index += emitter->max_size;
		}
		if (dist < minDist) {
			minDist = dist;
			iterator = 0;
		}

		if (iterator > 250) {
			break;
		}
		else {
			iterator++;
		}
	}

	return inGas;
}
