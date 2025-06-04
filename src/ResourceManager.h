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


struct ViewLight {
    unsigned int id;
    std::string type;
};

class ResourceManager
{
private:
	//modele z unikalnym id logicznym
	std::unordered_map<unsigned int, std::shared_ptr<Model>> models;
	//tekstury (nie modeli) z unikalnym id logicznym
	std::unordered_map<unsigned int, std::shared_ptr<Texture>> textures;
    //sprite z wyszukiwaniem po nazwie
    //std::unordered_map<std::string, std::shared_ptr<Sprite>> sprites;
    //swiatla z unikalnym id logicznym
    std::unordered_map<unsigned int, std::shared_ptr<ViewLight>> lights;

    ResourceManager() {

        shader = new Shader(vertexPath, fragmentPath);
        shader_line = new Shader(vertexPath_line, fragmentPath_line, geometryPath_line);
        shader_tile = new Shader(vertexPath_tile, fragmentPath);
        shader_instanced = new Shader(vertexPath_gas, fragmentPath_gas);
        shader_instanced_outline = new Shader(vertexPath_instanced, fragmentPath_outline);
        shader_outline = new Shader(vertexPath, fragmentPath_outline);
        shader2D = new Shader(triangleVertexPath, triangleFragmentPath);
        shader_shadow = new Shader(vertexPath_shadow, fragmentPath_shadow);
        shader_text = new Shader(vertexPath_text, fragmentPath_text);
        shader_background = new Shader(vertexPath_text, fragmentPath_background);
        shader_particle = new Shader(vertexPath_particle, fragmentPath_particle);
        
        shader_PostProcess_pass = new Shader(postPath_vert, postPath_pass);
        shader_PostProcess_ssao = new Shader(postPath_vert, postPath_ssao);
        shader_PostProcess_ssao_blur = new Shader(postPath_vert, postPath_ssao_blur);
        shader_PostProcess_ssao_composite = new Shader(postPath_vert, postPath_ssao_composite);
        shader_PostProcess_bloom = new Shader(postPath_vert, postPath_bloom);
        shader_PostProcess_gaussian_blur = new Shader(postPath_vert, postPath_gaussian_blur);
        shader_PostProcess_bloom_composite = new Shader(postPath_vert, postPath_bloom_composite);
        shader_PostProcess_crt = new Shader(postPath_vert, postPath_crt);
        shader_PostProcess_noise = new Shader(postPath_vert, postPath_noise);

    }
    ~ResourceManager() {

        delete shader;
        delete shader_line;
        delete shader_tile;
        delete shader_instanced;
        delete shader_instanced_outline;
        delete shader_outline;
        delete shader2D;
        delete shader_shadow;
        delete shader_text;
        delete shader_background;
        delete shader_particle;
        
        delete shader_PostProcess_pass;
        delete shader_PostProcess_ssao;
        delete shader_PostProcess_ssao_blur;
        delete shader_PostProcess_ssao_composite;
        delete shader_PostProcess_bloom;
        delete shader_PostProcess_gaussian_blur;
        delete shader_PostProcess_bloom_composite;
        delete shader_PostProcess_crt;
        delete shader_PostProcess_noise;

    }

    const char* vertexPath = "res/shaders/basic.vert";
    const char* vertexPath_line = "res/shaders/lines.vert";
    const char* geometryPath_line = "res/shaders/lines.geo";
    const char* fragmentPath_line = "res/shaders/lines.frag";
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
    const char* vertexPath_particle = "res/shaders/particle.vert";
    const char* fragmentPath_particle = "res/shaders/particle.frag";
    const char* vertexPath_gas = "res/shaders/gas.vert";
    const char* fragmentPath_gas = "res/shaders/gas.frag";
    const char* geometryPath_gas = "res/shaders/gas.geo";

    const char* postPath_vert = "res/shaders/pp_vert.vert";
    const char* postPath_pass = "res/shaders/pp_pass.frag";
    //const char* postPath_pass = "res/shaders/debug.frag";
    const char* postPath_ssao = "res/shaders/pp_ssao.frag";
    const char* postPath_ssao_blur = "res/shaders/pp_ssao_blur.frag";
    const char* postPath_ssao_composite = "res/shaders/pp_ssao_composite.frag";
    const char* postPath_bloom = "res/shaders/pp_bloom.frag";
    const char* postPath_gaussian_blur = "res/shaders/pp_blur.frag";
    const char* postPath_bloom_composite = "res/shaders/pp_bloom_composite.frag";
    const char* postPath_crt = "res/shaders/pp_crt.frag";
    const char* postPath_noise = "res/shaders/pp_noise.frag";

public:

    // Normal shaders

    Shader* shader;
    Shader* shader_line;
    Shader* shader_tile;
    Shader* shader_instanced;
    Shader* shader_instanced_outline;
    Shader* shader_outline;
    Shader* shader2D;
    Shader* shader_shadow;
    Shader* shader_text;
    Shader* shader_background;
    Shader* shader_particle;

    // Post process shaders

    Shader* shader_PostProcess_pass; // pass - just draw framebuffer with no changes
    Shader* shader_PostProcess_ssao; // ambient occlusion
    Shader* shader_PostProcess_ssao_blur; // ambient occlusion
    Shader* shader_PostProcess_ssao_composite; // ambient occlusion
    Shader* shader_PostProcess_bloom; // bloom
    Shader* shader_PostProcess_gaussian_blur;
    Shader* shader_PostProcess_bloom_composite;
    Shader* shader_PostProcess_crt;
    Shader* shader_PostProcess_noise;

	static ResourceManager& Instance() {
		static ResourceManager instance;
		return instance;
	}



	//path wskazuje sciezke do folderu zawierajacego zasoby
	void init(const char* path = "res/");

	std::shared_ptr<Model> getModel(unsigned int id);
	std::unordered_map<unsigned int, std::shared_ptr<Model>> getModels();
	std::unordered_map<unsigned int, std::shared_ptr<Texture>> getTextures();
    std::unordered_map<unsigned int, std::shared_ptr<ViewLight>> getLights();
	std::shared_ptr<Texture> getTexture(unsigned int id);
	//std::shared_ptr<Sprite> getSprite(const std::string& name);
    std::shared_ptr<ViewLight> getLight(unsigned int id);

};

#endif // !RESOURCE_MANAGER_H

