#include "ResourceManager.h"

#include <set>

#include "System/ServiceLocator.h"
#include "HUD/Sprite.h"
#include "Basic/Model.h"
#include "Basic/Mesh.h"

void ResourceManager::init(const char* path)
{

	drawStartWindow();

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

		max_models = resourceData["models"].size();

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
				loaded_models++;
				if (highest_id < id) highest_id = id;
				drawStartWindow();
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
					float scale_factor = 1.f;
					if (model.contains("scale_factor")) scale_factor = model["scale_factor"].get<float>();
					for (int i = 0; i < textures.size(); i++) {
						textures_ptr.push_back(getTexture(textures[i].get<unsigned int>()));
					}
					shared_ptr<Model> modelPtr = std::make_shared<Model>(std::move(textures_ptr), id, type, scale_factor);
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

std::shared_ptr<ViewLight> ResourceManager::getLight(unsigned int id)
{
	if (lights.find(id) != lights.end()) {
		return lights[id];
	}
	else {
		std::cerr << "Light with ID " << id << " not found!" << std::endl;
		return nullptr;
	}
}

void ResourceManager::drawStartWindow()
{
	unsigned int start_texture = textureFromFile("res/sprites/MenuEasterEggTransparent_loading.png", false);

	const float vertices[] = {
		-1.0f,  -1.0f,  0.0f, 1.0f,
		-1.0f, 1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 1.0f,
		1.0f,  -1.0f,  1.0f, 1.0f,
		 -1.0f, 1.0f,  0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 0.0f
	};

	float start_x = 535.0f / 1920.0f * 2.0f - 1.0f;
	float start_y = 1.f - 755.0f / 1080.0f * 2.0f;
	float end_x = 984.0f / 1920.0f * 2.0f - 1.0f;
	float end_y = 1.f - (729.0f / 1080.0f) * 2.0f;

	float scale = loaded_models / (float)(max_models + 1);
	float width_bar = (end_x - start_x) * scale;

	const float vertices_rect[] = {
		start_x, start_y,
		start_x, end_y,
		start_x + width_bar, start_y,
		start_x + width_bar, start_y,
		start_x, end_y,
		start_x + width_bar, end_y
	};

	//unsigned int VAO, VBO;
	if (start_screen_VAO == 0) {
		glGenVertexArrays(1, &start_screen_VAO);
		glGenBuffers(1, &start_screen_VBO);
		glBindVertexArray(start_screen_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, start_screen_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (start_bar_VAO == 0) {
		glGenVertexArrays(1, &start_bar_VAO);
		glGenBuffers(1, &start_bar_VBO);
		glBindVertexArray(start_bar_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, start_bar_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_rect), vertices_rect, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else {
		glBindVertexArray(start_bar_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, start_bar_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices_rect), vertices_rect);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader2D->use();
	shader2D->setInt("texture1", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, start_texture);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(start_bar_VAO);
	shader2D->setInt("hasTex", 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(start_screen_VAO);
	shader2D->setInt("hasTex", 1);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisable(GL_BLEND);

	ServiceLocator::getWindow()->updateWindow();
	
}

std::unordered_map<unsigned int, shared_ptr<ViewLight>> ResourceManager::getLights()
{
	return lights;
}

//std::shared_ptr<Sprite> ResourceManager::getSprite(const std::string& name)
//{
//	return sprites[name];
//}
