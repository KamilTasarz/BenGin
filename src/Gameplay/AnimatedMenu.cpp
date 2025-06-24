#include "AnimatedMenu.h"
#include "RegisterScript.h"
#include "GameManager.h"
#include "../System/GuiManager.h"
#include "../HUD/GuiButton.h"

REGISTER_SCRIPT(AnimatedMenu);

void AnimatedMenu::onStart()
{
	play = GuiManager::Instance().findButton(28);
	lead = GuiManager::Instance().findButton(30);
	background = GuiManager::Instance().findSprite(55);

	if (play) {
		play_x = play->button->pos.x;
	}
	if (lead) {
		lead_x = lead->button->pos.x;
	}
	exit = GuiManager::Instance().findButton(31);
	back = GuiManager::Instance().findButton(32);
	if (exit) {
		exit_x = exit->button->pos.x;
	}
	if (back) {
		back_x = back->button->pos.x;
	}

}

float my_lerp(float a, float b, float t) {
	return a + (b - a) * t;
}

float smoothstep(float edge0, float edge1, float x) {
	x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
	return x * x * (3 - 2 * x);
}

void AnimatedMenu::onUpdate(float deltaTime)
{
	if (isActive) {

		if (right) {
			alfa += deltaTime * 0.8f;
			
		}
		else {
			alfa -= deltaTime * 0.8f;
			
		}
		

		dx = std::lerp(0.f, -max_dx, alfa);

		if (right && dx < -max_dx) {
			dx = -max_dx;
			isActive = false;
			alfa = 1.f;
		}
		else if (!right && dx > 0.f) {
			dx = 0.f;
			isActive = false;
			alfa = 0.f;
		}


		if (play) {
			float w = play->button->w, h = play->button->h, y = play->button->pos.y;
			float x = play_x + dx;
			play->button->setParams(x, y, w, h);
		}
		if (lead) {
			float w = lead->button->w, h = lead->button->h, y = lead->button->pos.y;
			float x = lead_x + dx;
			lead->button->setParams(x, y, w, h);
		}
		if (exit) {
			float w = exit->button->w, h = exit->button->h, y = exit->button->pos.y;
			float x = exit_x + dx;
			exit->button->setParams(x, y, w, h);
		}
		if (back) {
			float w = back->button->w, h = back->button->h, y = back->button->pos.y;
			float x = back_x + dx;
			back->button->setParams(x, y, w, h);
		}
		if (background) {
			background->pos.x = dx;
		}
	}
}
