#pragma once

#include "../config.h"

class Sprite;
class Text;

enum ObjectType {
	SpriteType, TextType
};

class GuiObject {
public:
	int order_id;
	virtual ~GuiObject() = 0;
	virtual void render() = 0;
	virtual ObjectType getType() = 0;
};

class SpriteObject : public GuiObject {
public:
	std::weak_ptr<Sprite> sprite;
	glm::vec2 pos;
	float size;
	
	unsigned int id;
	SpriteObject(std::shared_ptr<Sprite> s, glm::vec2 pos, float size, int order_id, unsigned int id)
		: sprite(s), pos(pos), size(size), id(id) {
		this->order_id = order_id;
	}
	~SpriteObject() override = default;
	void render() override;
	ObjectType getType() override;
};

class TextObject : public GuiObject {
public:
	std::weak_ptr<Text> text;
	glm::vec2 pos;
	std::string value;
	glm::vec3 color = glm::vec3(0.f);
	unsigned int id;
	TextObject(std::shared_ptr<Text> t, glm::vec2 pos, std::string value, int order_id, unsigned int id, glm::vec3 color = { 0.f, 0.f, 0.f })
		: text(t), pos(pos), value(value), color(color), id(id) {
		this->order_id = order_id;
	}
	
	~TextObject() override = default;
	void render() override;
	ObjectType getType() override;
};

enum Text_names {
	ARIAL_48
};

enum Sprite_names {
	PIRATE
};

class GuiManager
{
private:
	std::unordered_map<unsigned int, std::shared_ptr<Sprite>> sprites;
	std::unordered_map<unsigned int, std::shared_ptr<Text>> texts;

	unsigned int max_id;
	std::vector<unsigned int> free_ids;

	std::vector<GuiObject*> objects;
	
public:
	static GuiManager& Instance() {
		static GuiManager instance;
		return instance;
	}

	void init();
	void update(float delta_time);
	void draw();

	// add text object to render
	void text(std::string value, float x, float y, Text_names text_id, int order_id = -1);

	// add sprite or animated sprite object to render
	void sprite(float x, float y, float size, Sprite_names sprite_id, int order_id = -1);

	TextObject* findText(unsigned int id);
	SpriteObject* findSprite(unsigned int id);

	void deleteText(unsigned int id);
	void deleteSprite(unsigned int id);
};

