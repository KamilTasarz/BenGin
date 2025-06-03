#include "GuiManager.h"
#include "../Text/Text.h"
#include "../HUD/Sprite.h"
#include "../ResourceManager.h"

void GuiManager::init(const char* path)
{
	/*std::string pathStr(path);
	if (fs::exists(pathStr + "config.json")) {
		std::ifstream file(pathStr + "config.json");
		if (!file) {
			std::cerr << "Błąd: Nie można otworzyć pliku JSON!\n";
		}

		json spriteData;
		file >> spriteData;
		file.close();

		string texturesPath = "sprites/";
		if (spriteData.contains("sprites")) {

			for (json sprite : spriteData["sprites"]) {
				const char* name = sprite["path"].get<string>().c_str();
				
				shared_ptr<Texture> texture = std::make_shared<Texture>(id, type, pathStr + texturesPath + name);
				textures[logic_id] = texture;
				highest_id_text = std::max(highest_id_text, logic_id);
				cout << "Texture loaded: " << pathStr + texturesPath + name << endl;
			}
		}

		string modelsPath = "models/";
		if (resourceData.contains("models")) {

			for (json model : resourceData["models"]) {
				std::string directory = model["directory"].get<string>();
				knownModelDirs.insert(directory);
				std::string name = model["name"].get<string>();
				unsigned int id = model["id"].get<unsigned int>();
				shared_ptr<Model> modelPtr;
				if (model.contains("move_origin")) {
					modelPtr = std::make_shared<Model>(path + modelsPath + directory + name, id, model["move_origin"]);
				}
				else {
					modelPtr = std::make_shared<Model>(path + modelsPath + directory + name, id);
				}
				models[id] = modelPtr;
				cout << "Model loaded: " << path + modelsPath + directory + name << endl;
				if (highest_id < id) highest_id = id;
			}

		}

		if (resourceData.contains("builtin_models")) {

			for (json model : resourceData["builtin_models"]) {
				std::string type = model["type"].get<string>();
				unsigned int id = model["id"].get<unsigned int>();

				if (type == "directional_light") {
					shared_ptr<ViewLight> light = std::make_shared<ViewLight>(id, type);
					lights[id] = light;
					std::cout << "Light loaded: " << type << " id: " << id << std::endl;
				}
				else if (type == "point_light") {
					shared_ptr<ViewLight> light = std::make_shared<ViewLight>(id, type);
					lights[id] = light;
					std::cout << "Light loaded: " << type << " id: " << id << std::endl;
				}
				else {

					json textures = model["textures_id"];
					std::vector<shared_ptr<Texture>> textures_ptr;
					for (int i = 0; i < textures.size(); i++) {
						textures_ptr.push_back(getTexture(textures[i].get<unsigned int>()));
					}
					shared_ptr<Model> modelPtr = std::make_shared<Model>(std::move(textures_ptr), id, type);
					models[id] = modelPtr;
					cout << "Model loaded: " << type << " id: " << id << endl;
					if (highest_id < id) highest_id = id;

				}
			}
		}

		for (const auto& dirEntry : fs::directory_iterator(pathStr + modelsPath)) {
			if (dirEntry.is_directory()) {
				std::string folderName = dirEntry.path().filename().string();
				if (knownModelDirs.find(folderName + "/") == knownModelDirs.end()) {

					for (const auto& file : fs::directory_iterator(dirEntry.path())) {
						if (file.path().extension() == ".obj" || file.path().extension() == ".gltf") {
							std::string fileName = file.path().filename().string();
							unsigned int newId = highest_id + 1;

							highest_id++;

							resourceData["models"].push_back({
								{ "directory", folderName + "/" },
								{ "name", fileName },
								{ "id", newId }
								});

							shared_ptr<Model> modelPtr = std::make_shared<Model>(pathStr + modelsPath + folderName + "/" + fileName, newId);
							models[newId] = modelPtr;
							std::cout << "[NEW] Model found and loaded: " << folderName << "/" << fileName << "\n";
							break;
						}
					}
				}
			}
		}

		for (const auto& file : fs::directory_iterator(pathStr + texturesPath)) {
			if (file.is_regular_file()) {
				std::string texName = file.path().filename().string();
				if (knownTextures.find(texName) == knownTextures.end()) {

					unsigned int newId = highest_id_text + 1;
					highest_id_text++;
					std::string type = "diffuse"; //wiekszosc jest diffuse

					unsigned int id = textureFromFile(texName.c_str(), pathStr + texturesPath);
					shared_ptr<Texture> texPtr = std::make_shared<Texture>(id, type, pathStr + texturesPath + texName);
					textures[newId] = texPtr;

					resourceData["textures"].push_back({
						{ "name", texName },
						{ "type", type },
						{ "id", newId }
						});

					std::cout << "[NEW] Texture found and loaded: " << texName << "\n";
				}
			}
		}
		/*if (resourceData.contains("sprites")) {
			for (json sprite : resourceData["sprites"]) {
				string type = sprite["type"].get<string>();
				if (type._Equal("sprite")) {
					unique_ptr<Sprite> spritePtr = std::make_unique<Sprite>(sprite["width"].get<float>(), sprite["height"].get<float>(), sprite["path"].get<string>(), sprite["x"].get<float>(), sprite["y"].get<float>(), sprite["scale"].get<float>());
				}
				else if (type._Equal("animated_sprite")) {

				}
			}
		}*/

	/*std::ofstream outFile(pathStr + "config.json");
	if (outFile.is_open()) {
		outFile << resourceData.dump(4); // 4 spaces for indentation
		outFile.close();
	}
	else {
		std::cerr << "Error opening file for writing: " << pathStr + "config.json" << std::endl;
	}
}*/
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
	std::sort(objects.begin(), objects.end(), [&](const GuiObject& a, const GuiObject& b) {
		return a.order_id < b.order_id;
		});


	for (GuiObject* o : objects) {
		o->render();
	}
}

void GuiManager::text(std::string value, float x, float y, Text_names text_id, int order_id)
{
	if (free_ids.empty()) {
		objects.push_back(new TextObject(texts[text_id], glm::vec2(x, y), value, order_id, max_id));
		max_id++;
	}
	else {
		objects.push_back(new TextObject(texts[text_id], glm::vec2(x, y), value, order_id, free_ids.back()));
		free_ids.pop_back();
	}
}

void GuiManager::sprite(float x, float y, float size, Sprite_names sprite_id, int order_id)
{
	if (free_ids.empty()) {
		objects.push_back(new SpriteObject(sprites[sprite_id], glm::vec2(x, y), size, order_id, max_id));
		max_id++;
	}
	else {
		objects.push_back(new SpriteObject(sprites[sprite_id], glm::vec2(x, y), size, order_id, free_ids.back()));
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
