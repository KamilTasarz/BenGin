#include "ResourceManager.h"

#include <set>


#include "HUD/Sprite.h"
#include "Basic/Model.h"
#include "Basic/Mesh.h"

void ResourceManager::init(const char* path)
{
	std::string pathStr(path);
	if (fs::exists(pathStr + "config.json")) {
		std::ifstream file(pathStr + "config.json");
		if (!file) {
			std::cerr << "Błąd: Nie można otworzyć pliku JSON!\n";
		}

		unsigned int highest_id = 0;
		unsigned int highest_id_text = 0;

		json resourceData;
		file >> resourceData;
		file.close();

		std::set<std::string> knownModelDirs;
		std::set<std::string> knownTextures;

		string texturesPath = "textures/";
		if (resourceData.contains("textures")) {
			
			for (json texture : resourceData["textures"]) {
				std::string name = texture["name"].get<string>();
				std::string type = texture["type"].get<string>();
				knownTextures.insert(name);
				unsigned int logic_id = texture["id"].get<unsigned int>();
				unsigned int id = textureFromFile(name.c_str(), pathStr + texturesPath);
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
				shared_ptr<Model> modelPtr = std::make_shared<Model>(path + modelsPath + directory + name, id);
				models[id] = modelPtr;
				cout << "Model loaded: " << path + modelsPath + directory + name << endl;
				if (highest_id < id) highest_id = id;
			}

		}

		if (resourceData.contains("builtin_models")) {

			for (json model : resourceData["builtin_models"]) {
				std::string type = model["type"].get<string>();
				unsigned int id = model["id"].get<unsigned int>();
				json textures = model["textures_id"];
				const char* texture_names[4];
				for (int i = 0; i < textures.size(); i++) {
					texture_names[i] = getTexture(textures[i].get<unsigned int>()).get()->path.c_str();
				}
				shared_ptr<Model> modelPtr = std::make_shared<Model>(texture_names, textures.size(), id, type);
				models[id] = modelPtr;
				cout << "Model loaded: " << type << " id: " << id << endl;
				if (highest_id < id) highest_id = id;
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

		std::ofstream outFile(pathStr + "config.json");
		if (outFile.is_open()) {
			outFile << resourceData.dump(4); // 4 spaces for indentation
			outFile.close();
		}
		else {
			std::cerr << "Error opening file for writing: " << pathStr + "config.json" << std::endl;
		}
	}
}

std::shared_ptr<Model> ResourceManager::getModel(unsigned int id)
{
	if (models.find(id) != models.end()) {
		return models[id];
	}
	else {
		std::cerr << "Model with ID " << id << " not found!" << std::endl;
		return nullptr;
	}
}

std::unordered_map<unsigned int, shared_ptr<Model>> ResourceManager::getModels()
{
	return models;
}

std::unordered_map<unsigned int, shared_ptr<Texture>> ResourceManager::getTextures()
{
	return textures;
}

std::shared_ptr<Texture> ResourceManager::getTexture(unsigned int id)
{
	if (textures.find(id) != textures.end()) {
		return textures[id];
	}
	else {
		std::cerr << "Texture with ID " << id << " not found!" << std::endl;
		return nullptr;
	}
}

//std::shared_ptr<Sprite> ResourceManager::getSprite(const std::string& name)
//{
//	return sprites[name];
//}
