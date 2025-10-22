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
	isGamepadConnected = false;
	
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
	//nickname = GuiManager::Instance().findText(53);
	if (nick) {
		nick_x = nick->pos.x;
	}
	if (nickname) {
		nickname_x = nickname->pos.x;
	}
	//nick_column = GuiManager::Instance().findText(58);
	score_column = GuiManager::Instance().findText(59);
	num_column = GuiManager::Instance().findText(60);
	//nick_button = GuiManager::Instance().findButton(54);
	if (nick_column) {
		nick2_x = nick_column->pos.x;
		int i = 0;
		for (Player_stats& p : GameManager::instance().playerStats) {
			if (i >= 10) break;
			nick_column->value += "\n" + p.name;
			i++;
		}
	}
	if (score_column) {
		scr_x = score_column->pos.x;
		int i = 0;
		for (Player_stats& p : GameManager::instance().playerStats) {
			if (i >= 10) break;
			score_column->value += "\n" + std::to_string(p.score);
			i++;
		}
	}
	if (num_column) {
		num_x = num_column->pos.x;
		for (int i = 1; i < 10; i++) num_column->value += "\n" + std::to_string(i + 1) + ".";
	}
	top = GuiManager::Instance().findText(2);
	if (top) {
		top_x = top->pos.x;
	}
	checkbox_marked = GuiManager::Instance().findSprite(63);
	checkbox = GuiManager::Instance().findSprite(62);
	tutorial = GuiManager::Instance().findButton(69);
	check_button = GuiManager::Instance().findButton(64);
	if (checkbox_marked) {
		checkbox_marked->visible = GameManager::instance().tutorialActive;
		c1 = checkbox_marked->pos.x;
	}
	if (checkbox) {
		c2 = checkbox->pos.x;
	}
	if (tutorial) {
		t = tutorial->button->pos.x;
	}
	if (check_button) {
		b = check_button->button->pos.x;
	}
	returnToMenuButton = GuiManager::Instance().findButton(65);
	if (returnToMenuButton) {
		returnToMenuButton->visible = false;

	}
	rerun = GuiManager::Instance().findButton(66);
	if (rerun) {
		rerun->visible = false;
	}
	last_run = GuiManager::Instance().findText(67);
	if (last_run) {
		last_run->value = std::to_string(GameManager::instance().last_run.score);
		last_run->visible = false;
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
	isGamepadConnected = glfwJoystickIsGamepad(GLFW_JOYSTICK_1);

	if (checkbox_marked) {
		_tutorial = GameManager::instance().tutorialActive;
		checkbox_marked->visible = _tutorial;
	}

	if (isGamepadConnected) {

		bool ok_button = isPadButtonPressed(GLFW_GAMEPAD_BUTTON_A);

		if (buttonId == 0) {
			if (!GameManager::instance().end_screen) {
				if (isMenu) {
					if (play) {
						play->button->is_hovered = true;
						if (ok_button) {
							if (!pressed_ok_button) {
								play->button->on_click();
								pressed_ok_button = true;
							}
						}
						else {
							pressed_ok_button = false;
						}
					}
				}
				else {


					if (back) {
						back->button->is_hovered = true;
					}
					if (ok_button) {
						if (!pressed_ok_button) {
							back->button->on_click();
							isMenu = true;
							buttonId = 1;
							pressed_ok_button = true;
						}
					}
					else {
						pressed_ok_button = false;
					}

				}
			}
			else {
				if (rerun) {
					rerun->button->is_hovered = true;
				}
				if (ok_button) {
					if (!pressed_ok_button) {
						rerun->button->on_click();
						pressed_ok_button = true;
					}
				}
				else {
					pressed_ok_button = false;
				}

			}
		}
		else if (buttonId == 1) {
			if (!GameManager::instance().end_screen) {
				if (lead) {
					lead->button->is_hovered = true;

					if (ok_button) {
						if (!pressed_ok_button) {
							lead->button->on_click();
							isMenu = false;
							buttonId = 0;
							pressed_ok_button = true;
						}
					}
					else {
						pressed_ok_button = false;
					}


				}
			}
			else {
				if (returnToMenuButton) {
					returnToMenuButton->button->is_hovered = true;
				}
				if (ok_button) {
					if (!pressed_ok_button) {
						returnToMenuButton->button->on_click();
						pressed_ok_button = true;
					}
				}
				else {
					pressed_ok_button = false;
				}
			}
		}
		else if (buttonId == 2) {
			if (exit) {
				exit->button->is_hovered = true;
				if (ok_button) {
					if (!pressed_ok_button) {
						exit->button->on_click();
						pressed_ok_button = true;
					}
				}
				else {
					pressed_ok_button = false;
				}
			}
		}
		else if (buttonId == 3) {
			if (check_button) {
				check_button->button->is_hovered = true;
				if (tutorial) {
					tutorial->button->is_hovered = true;
				}
				if (ok_button) {
					if (!pressed_ok_button) {
						check_button->button->on_click();
						pressed_ok_button = true;
					}
				}
				else {
					pressed_ok_button = false;
				}
			}
		}
		else if (buttonId == 4) {
			if (nick_button) {
				nick_button->button->is_hovered = true;
				if (ok_button) {
					if (!pressed_ok_button) {
						nick_button->button->on_click();
						pressed_ok_button = true;
					}
				}
				else {
					pressed_ok_button = false;
				}
			}
		}

		//float axisX = getPadAxis(GLFW_GAMEPAD_AXIS_LEFT_X);
		float axisY = getPadAxis(GLFW_GAMEPAD_AXIS_LEFT_Y);
		//float axisX2 = getPadAxis(GLFW_GAMEPAD_AXIS_RIGHT_X);
		float axisY2 = getPadAxis(GLFW_GAMEPAD_AXIS_RIGHT_Y);

		float dead_zone = 0.6f;

		bool up = isPadButtonPressed(GLFW_GAMEPAD_BUTTON_DPAD_UP) || axisY < -dead_zone || axisY2 < - dead_zone;
		bool down = isPadButtonPressed(GLFW_GAMEPAD_BUTTON_DPAD_DOWN) || axisY > dead_zone || axisY2 > dead_zone;
		
		if (up) {
			if (!GameManager::instance().end_screen) {
				if (!pressed) {
					pressed = true;
					if (buttonId > 0) {
						buttonId--;
					}
					else {
						buttonId = 4;
					}
				}
			}
			else {
				if (!pressed) {
					pressed = true;
					if (buttonId > 0) {
						buttonId--;
					}
					else {
						buttonId = 1;
					}
				}
			}
		}
		else if (down) {
			if (!GameManager::instance().end_screen) {
				if (!pressed) {
					pressed = true;
					if (buttonId < 4) {
						buttonId++;
					}
					else {
						buttonId = 0;
					}
				}
			}
			else {
				if (!pressed) {
					pressed = true;
					if (buttonId < 1) {
						buttonId++;
					}
					else {
						buttonId = 0;
					}
				}
			}
		}
		else {
			pressed = false;
		}

		if (!isMenu) buttonId = 0;
		
	}

	if (!GameManager::instance().end_screen) {
		if (play)
			play->visible = true;
		if (lead)
			lead->visible = true;
		if (exit)
			exit->visible = true;
		if (nick) {
			nick->visible = true;
		}
		if (returnToMenuButton) {
			returnToMenuButton->visible = false;

		}
		if (nickname) {
			nickname->visible = true;
		}
		if (tutorial) {
			tutorial->visible = true;
		}
		if (nick_column) {
			float x = nick2_x + dx;
			nick_column->pos.x = x;
			nick_column->visible = true;
		}
		if (score_column) {
			float x = scr_x + dx;
			score_column->pos.x = x;
			score_column->visible = true;
		}
		if (num_column) {
			float x = num_x + dx;
			num_column->pos.x = x;
			num_column->visible = true;

		}
		if (tv) {
			tv->visible = true;
		}
		if (top) {
			float x = top_x + dx;
			top->pos.x = x;
			top->visible = true;
		}
		
		if (check_button) {
			check_button->visible = true;
		}
		
		if (checkbox) {
			checkbox->visible = true;
		}
		if (rerun) {
			rerun->visible = false;
		}
		if (last_run) {
			last_run->visible = false;
		}
	}

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
		if (check_button) {
			float w = check_button->button->w, h = check_button->button->h, y = check_button->button->pos.y;
			float x = b + dx;
			check_button->button->setParams(x, y, w, h);
		}
		if (checkbox_marked) {
			float x = c1 + dx;
			checkbox_marked->pos.x = x;
		}
		if (checkbox) {
			float x = c2 + dx;
			checkbox->pos.x = x;
		}
		if (tutorial) {
			float w = tutorial->button->w, h = tutorial->button->h, y = tutorial->button->pos.y;
			float x = t + dx;
			tutorial->button->setParams(x, y, w, h);
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
		if (tutorial) {
			tutorial->visible = false;
		}
		if (nick_column) {
			nick_column->visible = false;
		}
		if (score_column) {
			score_column->visible = false;
		}
		if (num_column) {
			num_column->visible = false;
		}
		if (top) {
			top->visible = false;
		}
		if (check_button) {
			check_button->visible = false;
		}
		if (checkbox_marked) {
			checkbox_marked->visible = false;
		}
		if (checkbox) {
			checkbox->visible = false;
		}
		if (!reverse) {
			if (ending) {
				tv->size = 1.f + 2.f * endAnimTimer;
				tv->pos.x = (-(750.f * 3.f) + 960.f) * endAnimTimer;
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
		else {
			if (ending) {
				tv->size = 2.f * endAnimTimer + 1.f;
				tv->pos.x = (-(750.f * 3.f) + 960.f) * endAnimTimer;
				tv->pos.y = (-(540.f * 3.f) + 540.f) * endAnimTimer;
			}
			endAnimTimer -= deltaTime;
			if (ending && endAnimTimer < 0.f) {
				ending = false;
				starting = true;
				endAnimTimer = endAnimEndBuffor;
			}
			if (starting && endAnimTimer < 0.f) {

				endAnim = false;
				ending = false;
				starting = false;
				endAnimTimer = 0.f;
				SceneManager::Instance().getCurrentScene()->alpha_anim = 1.f;
				SceneManager::Instance().getCurrentScene()->menu_anim = false;
				GameManager::instance().end = false;
				GameManager::instance().start = false;
				GameManager::instance().end_screen = false;
			}
		}
	}
	if (GameManager::instance().end_screen) {
		if (GameManager::instance().playDeathMusic) {
			GameManager::instance().playDeathMusic = false;
			MusicManager::instance().PlayDeathMusic();
		}

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
		if (tutorial) {
			tutorial->visible = false;
		}
		if (nick_column) {
			float x = nick2_x - max_dx + 200.f;
			nick_column->pos.x = x;
			nick_column->visible = true;
		}
		if (score_column) {
			float x = scr_x - max_dx + 200.f;
			score_column->pos.x = x;
			score_column->visible = true;
		}
		if (num_column) {
			float x = num_x - max_dx + 200.f;
			num_column->pos.x = x;
			num_column->visible = true;

		}
		if (tv) {
			tv->visible = false;
		}
		if (top) {
			float x = top_x - max_dx + 200.f;
			top->pos.x = x;
			top->visible = true;
		}
		if (returnToMenuButton) {
			returnToMenuButton->visible = true;
		}
		if (check_button) {
			check_button->visible = false;
		}
		if (checkbox_marked) {
			checkbox_marked->visible = false;
		}
		if (checkbox) {
			checkbox->visible = false;
		}
		if (rerun) {
			rerun->visible = true;
		}
		if (last_run) {
			last_run->value = "Last run: " + std::to_string(GameManager::instance().place) + ".: " + std::to_string(GameManager::instance().last_run.score);
			last_run->visible = true;
		}
	}
}

bool AnimatedMenu::isPadButtonPressed(int button)
{

	if (!glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) return false;
	GLFWgamepadstate state;
	if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
		return state.buttons[button] == GLFW_PRESS;
	}
	return false;

}

float AnimatedMenu::getPadAxis(int axis)
{

	if (!glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) return 0.0f;
	GLFWgamepadstate state;
	if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
		return state.axes[axis];
	}
	return 0.0f;

}
