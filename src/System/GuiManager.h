#pragma once

#include "../config.h"

class Sprite;
class Text;

enum ObjectType {
	SpriteType, TextType
};

class GuiObject {
public:
	int order_id = 0;
	bool visible = true;
	glm::vec2 pos = { 0.f, 0.f };
	unsigned int id = 0;

	virtual ~GuiObject() {}
	virtual void render() = 0;
	virtual ObjectType getType() = 0;
};

class SpriteObject : public GuiObject {
public:
	std::weak_ptr<Sprite> sprite;
	float size;
	unsigned int sprite_id;

	SpriteObject(std::shared_ptr<Sprite> s, unsigned int s_id, glm::vec2 pos, float size, int order_id, unsigned int id)
		: sprite(s), size(size), sprite_id(s_id) {
		this->pos = pos;
		this->order_id = order_id;
		this->id = id;
	}
	~SpriteObject() override = default;
	void render() override;
	ObjectType getType() override;
};

class TextObject : public GuiObject {
public:
	std::weak_ptr<Text> text;
	std::string value;
	glm::vec3 color = glm::vec3(0.f);
	unsigned int text_id;
	int alignment = 0; // 0 - left, 1 - center, 2 - right
	TextObject(std::shared_ptr<Text> t, unsigned int t_id, glm::vec2 pos, std::string value, int order_id, unsigned int id, glm::vec3 color = { 0.f, 0.f, 0.f })
		: text(t), value(value), color(color), text_id(t_id) {
		this->pos = pos;
		this->order_id = order_id;
		this->id = id;
	}
	~TextObject() override = default;
	void render() override;
	ObjectType getType() override;
};

enum Text_names {
	ARIAL_16, 
	ARIAL_32, 
	ARIAL_48, 
	ARIAL_64,
	VCR_64,
	VCR_48,
	VCR_32
};

enum Sprite_names {
	HEART,
	PIRATE,
	GHOST,
	CORNERS,
	CHEESE
};

class GuiManager
{
private:
	std::unordered_map<unsigned int, std::shared_ptr<Sprite>> sprites;
	std::unordered_map<unsigned int, std::shared_ptr<Text>> texts;

	std::vector<GuiObject*> objects;
	
public:
	unsigned int max_id;
	std::vector<unsigned int> free_ids;

	static GuiManager& Instance() {
		static GuiManager instance;
		return instance;
	}

	void init(const char* path = "res/");
	void update(float delta_time);
	void draw();

	std::vector<GuiObject*>& getObjects() {
		//std::sort(objects.begin(), objects.end(), [&](GuiObject* a, GuiObject* b) { return a->order_id < b->order_id; });
		return objects;
	}

	std::unordered_map<unsigned int, std::shared_ptr<Sprite>>& getSprites() {
		return sprites;
	}

	std::unordered_map<unsigned int, std::shared_ptr<Text>>& getText() {
		return texts;
	}

	// add text object to render
	void text(std::string value, float x, float y, Text_names text_id, glm::vec3 color = {0.f, 0.f, 0.f}, int order_id = -1, int id = -1, bool visible = true, int alignment = 0);

	// add sprite or animated sprite object to render
	void sprite(float x, float y, float size, Sprite_names sprite_id, int order_id = -1, int id = -1, bool visible = true);

	TextObject* findText(unsigned int id);
	SpriteObject* findSprite(unsigned int id);

	void deleteText(unsigned int id);
	void deleteSprite(unsigned int id);


};

