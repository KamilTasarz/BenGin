#pragma once
#include "../config.h"

class TextObject;
class SpriteObject;
enum Text_names;
enum Sprite_names;

class GuiButton
{
public:
	bool is_invicible, is_pressed, is_hovered;
	glm::vec2 pos;
	float w, h;
	std::unique_ptr<TextObject> text_object;
	std::unique_ptr<SpriteObject> sprite_base;
	std::unique_ptr<SpriteObject> sprite_hovered;

	std::function<void()> on_click;

	GuiButton(float x, float y, float w, float h, std::string text_value, Text_names font_id, Sprite_names base_sprite_id, Sprite_names hovered_sprite_id, unsigned int id, int order_id = 0);

	void attachFunction(std::function<void()> onClick);

	void render();

	void update();
};

