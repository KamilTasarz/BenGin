#include "GuiButton.h"
#include "../System/ServiceLocator.h"
#include "../System/GuiManager.h"

GuiButton::GuiButton(float x, float y, float w, float h, std::string text_value, Text_names font_id, Sprite_names base_sprite_id, Sprite_names hovered_sprite_id, unsigned int id, int order_id)
{
	pos = glm::vec2(x, y);
	this->w = w;
	this->h = h;
	is_invicible = false;
	is_hovered = false;
	is_pressed = false;

	text_object = make_unique<TextObject>(GuiManager::Instance().getText()[(unsigned int)font_id], (unsigned int)font_id, glm::vec2(x + w / 2, y), text_value, order_id, id);
	sprite_base = make_unique<SpriteObject>(GuiManager::Instance().getSprites()[(unsigned int)hovered_sprite_id], (unsigned int)hovered_sprite_id, pos, 1.f, order_id, id);
	sprite_hovered = make_unique<SpriteObject>(GuiManager::Instance().getSprites()[(unsigned int)base_sprite_id], (unsigned int)base_sprite_id, pos, 1.f, order_id, id);
	
}

void GuiButton::attachFunction(std::function<void()> onClick)
{
	on_click = onClick;
}

void GuiButton::render()
{
	if (!is_invicible) {
		if (is_hovered) {
			sprite_hovered->render();
		}
		else {
			sprite_base->render();
		}
		text_object->render();
	}
}

void GuiButton::update()
{
	glm::vec2 mousePOS = ServiceLocator::getWindow()->getMousePosition();
	if (mousePOS.x > pos.x && mousePOS.x < pos.x + w && mousePOS.y > pos.y && mousePOS.y < pos.y + h) {
		is_hovered = true;
	}
	else {
		is_hovered = false;
	}

	if (glfwGetMouseButton(ServiceLocator::getWindow()->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && is_hovered && !is_pressed) {
		if (!is_pressed) {
			is_pressed = true;
			on_click();
		}
	}
	else {
		is_pressed = false;
	}
}
