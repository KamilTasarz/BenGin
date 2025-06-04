#include "GuiManager.h"
#include "../Text/Text.h"
#include "../HUD/Sprite.h"
#include "../ResourceManager.h"
#include "../System/Window.h"

void GuiManager::init(const char* path)
{
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
}

void GuiManager::draw()
{
	std::sort(objects.begin(), objects.end(), [&](GuiObject* a, GuiObject* b) {
		return a->order_id < b->order_id;
		});


	for (GuiObject* o : objects) {
		o->render();
	}
}

void GuiManager::text(std::string value, float x, float y, Text_names text_id, int order_id)
{
	if (free_ids.empty()) {
		objects.push_back(new TextObject(texts[text_id], text_id, glm::vec2(x, y), value, order_id, max_id));
		max_id++;
	}
	else {
		objects.push_back(new TextObject(texts[text_id], text_id, glm::vec2(x, y), value, order_id, free_ids.back()));
		free_ids.pop_back();
	}
}

void GuiManager::sprite(float x, float y, float size, Sprite_names sprite_id, int order_id)
{
	if (free_ids.empty()) {
		objects.push_back(new SpriteObject(sprites[sprite_id], sprite_id, glm::vec2(x, y), size, order_id, max_id));
		max_id++;
	}
	else {
		objects.push_back(new SpriteObject(sprites[sprite_id], sprite_id, glm::vec2(x, y), size, order_id, free_ids.back()));
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
	text.lock()->renderText(value, pos.x, pos.y, *ResourceManager::Instance().shader_background, color);
}

ObjectType TextObject::getType()
{
	return TextType;
}
