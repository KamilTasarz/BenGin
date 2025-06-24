#include "Cage.h"
#include "../Basic/Node.h"
#include "RegisterScript.h"
#include "GameManager.h"
#include "../Basic/Animator.h"
#include "PlayerController.h"

REGISTER_SCRIPT(Cage);

void Cage::onAttach(Node* owner)
{
	this->owner = owner;
}

void Cage::onDetach()
{
	owner = nullptr;
}

void Cage::onStart()
{
}

void Cage::onUpdate(float deltaTime)
{
	if (!owner->is_animating) {
		owner->animator->playAnimation(owner->pModel->getAnimationByName("CageOpen"), false);
		owner->is_animating = true;
		owner->animator->current_animation->speed = openingSpeed;
	}

	if (timer < 0.f && !isOpen) {
		owner->getChildById(0)->setPhysic(false);
		isOpen = true;
	}
	else {
		timer -= deltaTime;
	}
}
