#include "RenderSystem.h"
#include "../ResourceManager.h"
#include "../Basic/Node.h"
#include "../Basic/Model.h"
#include "../Basic/Animator.h"
#include "../Basic/Mesh.h"

void RenderSystem::addAnimatedObject(Node* obj)
{
	RenderObject renderObj;
	renderObj.model = obj->pModel;
	if (!obj->textures.empty()) renderObj.textures = obj->textures; 
	renderObj.color = glm::vec4(obj->color);
	renderObj.modelMatrix = obj->transform.getModelMatrix();
	renderObj.animator = obj->animator;
	animatedObjects.push_back(renderObj);
}

void RenderSystem::addStaticObject(Node* obj)
{
	RenderObject renderObj;
	renderObj.color = glm::vec4(obj->color);
	renderObj.model = obj->pModel;
	if (!obj->textures.empty()) renderObj.textures = obj->textures;
	renderObj.modelMatrix = obj->transform.getModelMatrix();
	renderObj.animator = nullptr; // Static objects typically don't have animators
	staticObjects.push_back(renderObj);
}

void RenderSystem::addTileObject(Node* obj)
{
	RenderObject renderObj;
	renderObj.model = obj->pModel;
	renderObj.color = glm::vec4(obj->color);
	if (!obj->textures.empty()) renderObj.textures = obj->textures;
	renderObj.modelMatrix = obj->transform.getModelMatrix();
	renderObj.animator = nullptr; // Tile objects typically don't have animators
	renderObj.tile_scale = obj->pModel->tile_scale;
	tileObjects.push_back(renderObj);
}

void RenderSystem::render()
{
	ResourceManager::Instance().shader_tile->use();
	ResourceManager::Instance().shader_tile->setInt("is_light", 0);
	ResourceManager::Instance().shader_tile->setInt("is_animating", 0);
	for (const auto& obj : tileObjects) {
		glm::vec4 color = glm::vec4(obj.color);
		ResourceManager::Instance().shader_tile->setVec4("color", color);
		ResourceManager::Instance().shader_tile->setFloat("tile_scale", obj.tile_scale);
		ResourceManager::Instance().shader_tile->setMat4("model", obj.modelMatrix);
		if (obj.textures.empty()) {
			obj.model->Draw(*ResourceManager::Instance().shader_tile);
		}
		else {
			obj.model->Draw(*ResourceManager::Instance().shader_tile, obj.textures);
		}
	}

	ResourceManager::Instance().shader->use();
	ResourceManager::Instance().shader->setInt("is_light", 0);
	
	ResourceManager::Instance().shader->setInt("is_animating", 1);
	for (const auto& obj : animatedObjects) {
		
		ResourceManager::Instance().shader->setMat4("model", obj.modelMatrix);
		glm::vec4 color = glm::vec4(obj.color);
		ResourceManager::Instance().shader->setVec4("color", color);

		auto& f = obj.animator->final_bone_matrices;
		for (int i = 0; i < f.size(); ++i) {
			ResourceManager::Instance().shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", f[i]);
		}
		if (obj.textures.empty()) {
			obj.model->Draw(*ResourceManager::Instance().shader);
		}
		else {
			obj.model->Draw(*ResourceManager::Instance().shader, obj.textures);
		}
		
	}
	ResourceManager::Instance().shader->setInt("is_animating", 0);
	for (const auto& obj : staticObjects) {
		ResourceManager::Instance().shader->setMat4("model", obj.modelMatrix);
		glm::vec4 color = glm::vec4(obj.color);
		ResourceManager::Instance().shader->setVec4("color", color);
		if (obj.textures.empty()) {
			obj.model->Draw(*ResourceManager::Instance().shader);
		}
		else {
			obj.model->Draw(*ResourceManager::Instance().shader, obj.textures);
		}
	}


	
	
}

void RenderSystem::renderShadows()
{
	ResourceManager::Instance().shader_shadow->use();
	ResourceManager::Instance().shader_shadow->setInt("is_animating", 1);
	for (const auto& obj : animatedObjects) {
		ResourceManager::Instance().shader_shadow->setMat4("model", obj.modelMatrix);
		auto& f = obj.animator->final_bone_matrices;
		for (int i = 0; i < f.size(); ++i) {
			ResourceManager::Instance().shader_shadow->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", f[i]);
		}
		obj.model->Draw(*ResourceManager::Instance().shader_shadow);
	}
	ResourceManager::Instance().shader_shadow->setInt("is_animating", 0);
	for (const auto& obj : staticObjects) {
		ResourceManager::Instance().shader_shadow->setMat4("model", obj.modelMatrix);
		obj.model->Draw(*ResourceManager::Instance().shader_shadow);
	}

	for (const auto& obj : tileObjects) {
		ResourceManager::Instance().shader_shadow->setMat4("model", obj.modelMatrix);
		obj.model->Draw(*ResourceManager::Instance().shader_shadow);
	}
}

void RenderSystem::clear() {
	animatedObjects.clear();
	staticObjects.clear();
	tileObjects.clear();
}
