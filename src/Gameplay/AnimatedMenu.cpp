#include "AnimatedMenu.h"
#include "RegisterScript.h"
#include "GameManager.h"
#include "../System/GuiManager.h"
#include "../System/SceneManager.h"
#include "MusicManager.h"
#include "../HUD/GuiButton.h"

REGISTER_SCRIPT(AnimatedMenu);

void AnimatedMenu::onStart()
{
	play = GuiManager::Instance().findButton(28);
	lead = GuiManager::Instance().findButton(30);
	//background = GuiManager::Instance().findSprite(55);
	tv = GuiManager::Instance().findSprite(55);

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
	nick = GuiManager::Instance().findText(29);
	nickname = GuiManager::Instance().findText(53);
	if (nick) {
		nick_x = nick->pos.x;
	}
	if (nickname) {
		nickname_x = nickname->pos.x;
	}
	nick_column = GuiManager::Instance().findText(58);
	score_column = GuiManager::Instance().findText(59);
	num_column = GuiManager::Instance().findText(60);
	if (nick_column) {
		nick2_x = nick_column->pos.x;
		for (int i = 0; i < 10; i++) nick_column->value += "\nKAMIL";
	}
	if (score_column) {
		scr_x = score_column->pos.x;
		for (int i = 0; i < 10; i++) score_column->value += "\n1000";
	}
	if (num_column) {
		num_x = num_column->pos.x;
		for (int i = 1; i < 10; i++) num_column->value += "\n" + std::to_string(i + 1) + ".";
	}
	top = GuiManager::Instance().findText(2);
	if (top) {
		top_x = top->pos.x;
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
			alfa += deltaTime * spd;
			
		}
		else {
			alfa -= deltaTime * spd;
			
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
		if (nick) {
			float x = nick_x + dx;
			nick->pos.x = x;
		}
		if (nickname) {
			float x = nickname_x + dx;
			nickname->pos.x = x;
		}
		if (background) {
			background->pos.x = dx;
		}
		if (nick_column) {
			float x = nick2_x + dx;
			nick_column->pos.x = x;
		}
		if (score_column) {
			float x = scr_x + dx;
			score_column->pos.x = x;
		}
		if (num_column) {
			float x = num_x + dx;
			num_column->pos.x = x;
		}
		if (top) {
			float x = top_x + dx;
			top->pos.x = x;
		}
	}
	if (endAnim) {
		if (play)
			play->visible = false;
		if (lead)
			lead->visible = false;
		if (exit)
			exit->visible = false;
		if (nick) {
			nick->visible = false;
		}
		if (nickname) {
			nickname->visible = false;
		}
		if (ending) {
			tv->size = 1.f + 2.f * endAnimTimer;
			tv->pos.x = (- (750.f * 3.f) + 960.f) * endAnimTimer;
			tv->pos.y = (-(540.f * 3.f) + 540.f) * endAnimTimer;
		} 
		endAnimTimer += deltaTime;
		if (starting && endAnimTimer > endAnimStartBuffor) {
			ending = true;
			starting = false;
			endAnimTimer = 0.f;
		}
		if (ending && endAnimTimer > endAnimEndBuffor) {
			
			SceneManager::Instance().LateNext();

			endAnim = false;
			ending = false;
			starting = true;
			endAnimTimer = 0.f;
		}
	}
}
