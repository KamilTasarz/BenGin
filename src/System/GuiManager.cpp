#include "GuiManager.h"
#include "../Text/Text.h"
#include "../HUD/Sprite.h"
#include "../ResourceManager.h"
#include "../System/Window.h"
#include "../HUD/GuiButton.h"
#include "SceneManager.h"

void GuiManager::init(const char* path)
{
	objects.clear();
	sprites.clear();
	texts.clear();
	free_ids.clear();

	std::string pathStr(path);
	if (fs::exists(pathStr + "config.json")) {
		std::ifstream file(pathStr + "config.json");
		if (!file) {
			std::cerr << "Błąd: Nie można otworzyć pliku JSON!\n";
		}

		json spriteData;
		file >> spriteData;
		file.close();

		string spritePath = pathStr + "sprites/";
		if (spriteData.contains("sprites")) {

			for (json sprite_json : spriteData["sprites"]) {
				shared_ptr<Sprite> sprite;
				std::string type = sprite_json["type"];
				if (type == "Sprite") {

					std::string s = (spritePath + (std::string)sprite_json["name"][0]);
					const char* path = s.c_str();
					sprite = std::make_shared<Sprite>(WINDOW_WIDTH, WINDOW_HEIGHT, path, 0, 0, 1.f);
				}
				else {
					float duration = sprite_json["duration"].get<float>();
					int frames = sprite_json["frames"].get<int>();

					if (sprite_json["name"].size() == 1) {
						std::string s = (spritePath + (std::string)sprite_json["name"][0]);
						const char* path = s.c_str();
						int rows = sprite_json["rows"].get<int>();
						int frames = sprite_json["frames"].get<int>();
						int f_per_r = sprite_json["frames_per_row"].get<int>();
						sprite = std::make_shared<AnimatedSprite>(WINDOW_WIDTH, WINDOW_HEIGHT, duration, path, rows, f_per_r, frames, 0, 0, 1.f);
					}
					else {
						std::vector<std::string> strings;
						const char** paths = new const char* [frames];
						for (int i = 0; i < frames; i++) {
							strings.push_back((spritePath + (std::string)sprite_json["name"][i]));
							paths[i] = strings.back().c_str();
						}
						sprite = std::make_shared<AnimatedSprite>(WINDOW_WIDTH, WINDOW_HEIGHT, duration, paths, frames, 0, 0, 1.f);
						delete[] paths;
					}

					
				}

				sprites[sprite_json["id"].get<unsigned int>()] = sprite;

			}
		}
		string textPath = pathStr + "fonts/";
		if (spriteData.contains("texts")) {

			for (json text_json : spriteData["texts"]) {
				std::string s = (textPath + (std::string)text_json["name"]);
				const char* path = s.c_str();
				int size = text_json["size"].get<int>();
				shared_ptr<Text> text = make_shared<Text>(path, size);

				texts[text_json["id"].get<unsigned int>()] = text;
			}
		}

	}
}

void GuiManager::update(float delta_time)
{
	for (auto& s : sprites) {
		AnimatedSprite* a = dynamic_cast<AnimatedSprite*>(s.second.get());
		if (a) {
			a->update(delta_time);
		}
	}

	for (auto& o : objects) {
		ButtonObject* b = dynamic_cast<ButtonObject*>(o);
		if (b && b->button->on_click) {
			b->button->update();
			if (SceneManager::Instance().isSwitched()) break;
		}
	}
}

void GuiManager::draw()
{
	std::sort(objects.begin(), objects.end(), [&](GuiObject* a, GuiObject* b) {
		return a->order_id < b->order_id;
		});


	for (GuiObject* o : objects) {
		if (o->visible)
			o->render();
	}
}

void GuiManager::text(std::string value, float x, float y, Text_names text_id, glm::vec3 color, int order_id, int id, bool visible, int alignment)
{
	if (id != -1) {
		TextObject* t = new TextObject(texts[text_id], text_id, glm::vec2(x, y), value, order_id, id, color);
		t->visible = visible;
		t->alignment = alignment;
		objects.push_back(t);
	}
	else if (free_ids.empty()) {
		TextObject* t = new TextObject(texts[text_id], text_id, glm::vec2(x, y), value, order_id, max_id, color);
		t->visible = visible;
		t->alignment = alignment;
		objects.push_back(t);
		max_id++;
	}
	else {
		TextObject* t = new TextObject(texts[text_id], text_id, glm::vec2(x, y), value, order_id, free_ids.back(), color);
		t->visible = visible;
		t->alignment = alignment;
		objects.push_back(t);
		free_ids.pop_back();
	}
}

void GuiManager::sprite(float x, float y, float size, Sprite_names sprite_id, int order_id, int id, bool visible)
{
	if (id != -1) {
		SpriteObject* s = new SpriteObject(sprites[sprite_id], sprite_id, glm::vec2(x, y), size, order_id, id);
		s->visible = visible;
		objects.push_back(s);
	}
	else if (free_ids.empty()) {
		SpriteObject* s = new SpriteObject(sprites[sprite_id], sprite_id, glm::vec2(x, y), size, order_id, max_id);
		s->visible = visible;
		objects.push_back(s);
		max_id++;
	}
	else {
		SpriteObject* s = new SpriteObject(sprites[sprite_id], sprite_id, glm::vec2(x, y), size, order_id, free_ids.back());
		s->visible = visible;
		objects.push_back(s);
		free_ids.pop_back();
	}
}

void GuiManager::button(float x, float y, float w, float h, std::string text_value, Text_names font_id, Sprite_names base_sprite_id, Sprite_names hovered_sprite_id, int order_id, int id, bool visible)
{
	if (id != -1) {
		ButtonObject* s = new ButtonObject(x ,y, w, h, text_value, font_id, base_sprite_id, hovered_sprite_id, id, order_id);
		s->visible = visible;
		objects.push_back(s);
	}
	else if (free_ids.empty()) {
		ButtonObject* s = new ButtonObject(x, y, w, h, text_value, font_id, base_sprite_id, hovered_sprite_id, max_id, order_id);
		s->visible = visible;
		objects.push_back(s);
		max_id++;
	}
	else {
		ButtonObject* s = new ButtonObject(x, y, w, h, text_value, font_id, base_sprite_id, hovered_sprite_id, free_ids.back(), order_id);
		s->visible = visible;
		objects.push_back(s);
		free_ids.pop_back();
	}
}

TextObject* GuiManager::findText(unsigned int id)
{
	for (auto& o : objects) {
		if (o->getType() == TextType) {
			TextObject* t = static_cast<TextObject*>(o);
			if (t && t->id == id) return t;
		}
	}
	return nullptr;
}

SpriteObject* GuiManager::findSprite(unsigned int id)
{
	for (auto& o : objects) {
		if (o->getType() == SpriteType) {
			SpriteObject* t = static_cast<SpriteObject*>(o);
			if (t && t->id == id) return t;
		}
	}
	return nullptr;
}

ButtonObject* GuiManager::findButton(unsigned int id)
{
	for (auto& o : objects) {
		if (o->getType() == ButtonType) {
			ButtonObject* b = static_cast<ButtonObject*>(o);
			if (b && b->id == id) return b;
		}
	}
	return nullptr;
}

void GuiManager::deleteText(unsigned int id)
{
	TextObject* t = findText(id);
	if (t) {
		free_ids.push_back(id);
		auto it = std::find(objects.begin(), objects.end(), t);
		if (it != objects.end()) {
			objects.erase(it);
		}
		delete t;
	}
}

void GuiManager::deleteSprite(unsigned int id)
{
	SpriteObject* t = findSprite(id);
	if (t) {
		free_ids.push_back(id);
		auto it = std::find(objects.begin(), objects.end(), t);
		if (it != objects.end()) {
			objects.erase(it);
		}
		delete t;
	}
}

void GuiManager::deleteButton(unsigned int id)
{
	ButtonObject* b = findButton(id);
	if (b) {
		free_ids.push_back(id);
		auto it = std::find(objects.begin(), objects.end(), b);
		if (it != objects.end()) {
			objects.erase(it);
		}
		delete b;
	}
}

void SpriteObject::render()
{
	sprite.lock()->render(*ResourceManager::Instance().shader_background, pos.x, pos.y, size);
}

ObjectType SpriteObject::getType()
{
	return SpriteType;
}

void TextObject::render()
{
	text.lock()->renderText(value, pos.x, pos.y, *ResourceManager::Instance().shader_text, color, (Alignment) alignment);
}

ObjectType TextObject::getType()
{
	return TextType;
}

ButtonObject::ButtonObject(float x, float y, float w, float h, std::string text_value, Text_names font_id, Sprite_names base_sprite_id, Sprite_names hovered_sprite_id, unsigned int id, int order_id)
{
	button = make_unique<GuiButton>(x, y, w, h, text_value, font_id, base_sprite_id, hovered_sprite_id, id, order_id);
	this->pos = glm::vec2(x, y);
	this->order_id = order_id;
	this->id = id;
}

void ButtonObject::render()
{
	button->render();
}

ObjectType ButtonObject::getType()
{
	return ButtonType;
}

void ButtonObject::attach(std::function<void()> on_action, std::string name)
{
	button->attachFunction(on_action, name);
}
