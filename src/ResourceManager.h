#pragma once

#ifndef RESOURCE_MANAGER_H

#define RESOURCE_MANAGER_H

#include <unordered_map>

#include <nlohmann/json.hpp>


#include "Basic/Model.h"

#include <filesystem>
#include <iostream>


namespace fs = std::filesystem;

using json = nlohmann::json;

class ResourceManager
{
private:
	//modele z unikalnym id logicznym
	std::unordered_map<unsigned int, std::shared_ptr<Model>> models;
	//tekstury (nie modeli) z unikalnym id logicznym
	std::unordered_map<unsigned int, std::shared_ptr<Texture>> textures;

	ResourceManager() = default;
	~ResourceManager() = default;
public:

	static ResourceManager& Instance() {
		static ResourceManager instance;
		return instance;
	}



	//path wskazuje sciezke do folderu zawierajacego zasoby
	void init(const char* path = "res/");

	std::shared_ptr<Model> getModel(unsigned int id);
	std::unordered_map<unsigned int, shared_ptr<Model>> getModels();
	std::unordered_map<unsigned int, shared_ptr<Texture>> getTextures();
	std::shared_ptr<Texture> getTexture(unsigned int id);
};

#endif // !RESOURCE_MANAGER_H

