#pragma once

#ifndef RESOURCE_MANAGER_H

#define RESOURCE_MANAGER_H

#include <unordered_map>

#include <nlohmann/json.hpp>

#include <filesystem>
#include <iostream>

#include "Basic/Shader.h"

class Model;
struct Texture;

namespace fs = std::filesystem;

using json = nlohmann::json;

class ResourceManager
{
private:
	//modele z unikalnym id logicznym
	std::unordered_map<unsigned int, std::shared_ptr<Model>> models;
	//tekstury (nie modeli) z unikalnym id logicznym
	std::unordered_map<unsigned int, std::shared_ptr<Texture>> textures;
    //sprite z wyszukiwaniem po nazwie
    //std::unordered_map<std::string, std::shared_ptr<Sprite>> sprites;

    ResourceManager() {
        shader = new Shader(vertexPath, fragmentPath);
        shader_tile = new Shader(vertexPath_tile, fragmentPath);
        shader_instanced = new Shader(vertexPath_instanced, fragmentPath);
        shader_instanced_outline = new Shader(vertexPath_instanced, fragmentPath_outline);
        shader_outline = new Shader(vertexPath, fragmentPath_outline);
        shader2D = new Shader(triangleVertexPath, triangleFragmentPath);
        shader_shadow = new Shader(vertexPath_shadow, fragmentPath_shadow);
        shader_text = new Shader(vertexPath_text, fragmentPath_text);
        shader_background = new Shader(vertexPath_text, fragmentPath_background);
        shader_vhs = new Shader(vertexPath_vhs, fragmentPath_vhs);
    }
    ~ResourceManager() {
        delete shader;
        delete shader_tile;
        delete shader_instanced;
        delete shader_instanced_outline;
        delete shader_outline;
        delete shader2D;
        delete shader_shadow;
        delete shader_text;
        delete shader_background;
        delete shader_vhs;
    }



    const char* vertexPath = "res/shaders/basic.vert";
    const char* vertexPath_tile = "res/shaders/tile.vert";
    const char* vertexPath_instanced = "res/shaders/instanced.vert";
    const char* fragmentPath = "res/shaders/basic.frag";
    const char* vertexPath_shadow = "res/shaders/shadow.vert";
    const char* fragmentPath_shadow = "res/shaders/shadow.frag";
    const char* fragmentPath_outline = "res/shaders/outline.frag";
    const char* triangleVertexPath = "res/shaders/triangle.vert";
    const char* triangleFragmentPath = "res/shaders/triangle.frag";
    const char* vertexPath_text = "res/shaders/text.vert";
    const char* fragmentPath_text = "res/shaders/text.frag";
    const char* fragmentPath_background = "res/shaders/background.frag";
    const char* fragmentPath_vhs = "res/shaders/vhs.frag";
    const char* vertexPath_vhs = "res/shaders/vhs.vert";



public:

    Shader* shader;
    Shader* shader_tile;
    Shader* shader_instanced;
    Shader* shader_instanced_outline;
    Shader* shader_outline;
    Shader* shader2D;
    Shader* shader_shadow;
    Shader* shader_text;
    Shader* shader_background;
    Shader* shader_vhs;

	static ResourceManager& Instance() {
		static ResourceManager instance;
		return instance;
	}



	//path wskazuje sciezke do folderu zawierajacego zasoby
	void init(const char* path = "res/");

	std::shared_ptr<Model> getModel(unsigned int id);
	std::unordered_map<unsigned int, std::shared_ptr<Model>> getModels();
	std::unordered_map<unsigned int, std::shared_ptr<Texture>> getTextures();
	std::shared_ptr<Texture> getTexture(unsigned int id);
	//std::shared_ptr<Sprite> getSprite(const std::string& name);


};

#endif // !RESOURCE_MANAGER_H

