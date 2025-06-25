#include "GuiButton.h"
#include "../System/ServiceLocator.h"
#include "../System/GuiManager.h"
#include "Sprite.h"


GuiButton::GuiButton(float x, float y, float w, float h, std::string text_value, Text_names font_id, Sprite_names base_sprite_id, Sprite_names hovered_sprite_id, unsigned int id, int order_id)
{
	pos = glm::vec2(x, y);
	this->w = w;
	this->h = h;
	is_invincible = false;
	is_hovered = false;
	is_pressed = false;

	text_object = make_unique<TextObject>(GuiManager::Instance().getText()[(unsigned int)font_id], (unsigned int)font_id, glm::vec2(x + w / 2, y + h /3), text_value, order_id, id);
	text_object->alignment = 1; // Center alignment
	sprite_hovered = make_unique<SpriteObject>(GuiManager::Instance().getSprites()[(unsigned int)hovered_sprite_id], (unsigned int)hovered_sprite_id, pos, 1.f, order_id, id);
	sprite_base = make_unique<SpriteObject>(GuiManager::Instance().getSprites()[(unsigned int)base_sprite_id], (unsigned int)base_sprite_id, pos, 1.f, order_id, id);
	
}

void GuiButton::setParams(float x, float y, float w, float h)
{
	this->w = w;
	this->h = h;
	pos = glm::vec2(x, y);
	text_object->pos = glm::vec2(x + w / 2, y + h / 3);
	sprite_base->pos = pos;
	sprite_hovered->pos = pos;
}

void GuiButton::attachFunction(std::function<void()> onClick, std::string fun_name)
{
	on_click = onClick;
	this->fun_name = fun_name;
}

void GuiButton::render()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (!is_invincible) {
		if (is_hovered) {
			
			float sprite_w = sprite_hovered->sprite.lock()->sprite_w;
			float sprite_h = sprite_hovered->sprite.lock()->sprite_h;
			sprite_hovered->sprite.lock()->sprite_w = w;
			sprite_hovered->sprite.lock()->sprite_h = h;
			sprite_hovered->render();
			sprite_hovered->sprite.lock()->sprite_w = sprite_w;
			sprite_hovered->sprite.lock()->sprite_h = sprite_h;
			text_object->color = glm::vec4(0.f, 0.f, 0.f, 1.f);
			
		}
		else {
			
			float sprite_w = sprite_base->sprite.lock()->sprite_w;
			float sprite_h = sprite_base->sprite.lock()->sprite_h;
			sprite_base->sprite.lock()->sprite_w = w;
			sprite_base->sprite.lock()->sprite_h = h;
			sprite_base->render();
			sprite_base->sprite.lock()->sprite_w = sprite_w;
			sprite_base->sprite.lock()->sprite_h = sprite_h;
			text_object->color = glm::vec4(1.f);
			
		}
		text_object->render();
	}
	is_hovered = false;
}

void GuiButton::update()
{
	glm::vec2 mousePOS = ServiceLocator::getWindow()->getMousePosition();
	if (mousePOS.x > pos.x && mousePOS.x < pos.x + w && mousePOS.y > pos.y && mousePOS.y < pos.y + h) {
		is_hovered = true;
	}

	if (glfwGetMouseButton(ServiceLocator::getWindow()->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && is_hovered) {
		if (!is_pressed) {
			is_pressed = true;
			on_click();
		}
	}
	else {
		is_pressed = false;
	}
}
