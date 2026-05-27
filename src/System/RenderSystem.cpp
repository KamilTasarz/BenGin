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
	if (!obj->textures.empty()) renderObj.textures = &obj->textures;
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
	if (!obj->textures.empty()) renderObj.textures = &obj->textures;
	renderObj.modelMatrix = obj->transform.getModelMatrix();
	renderObj.animator = nullptr; // Static objects typically don't have animators
	staticObjects.push_back(renderObj);
}

void RenderSystem::addTileObject(Node* obj)
{
	RenderObject renderObj;
	renderObj.model = obj->pModel;
	renderObj.color = glm::vec4(obj->color);
	if (!obj->textures.empty()) renderObj.textures = &obj->textures;
	renderObj.modelMatrix = obj->transform.getModelMatrix();
	renderObj.animator = nullptr; // Tile objects typically don't have animators
	renderObj.tile_scale = obj->pModel->tile_scale;
	tileObjects.push_back(renderObj);
}

void RenderSystem::render()
{

	// Reference the singleton
	auto& r = ResourceManager::Instance();

	Shader* tileShader = r.shader_tile;
	Shader* animShader = r.shader;

	tileShader->use();
	tileShader->setInt("is_light", 0);
	tileShader->setInt("is_animating", 0);

	// Tiles
	for (const auto& obj : tileObjects) {

		glm::vec4 color = glm::vec4(obj.color);
		tileShader->setVec4("color", color);
		tileShader->setFloat("tile_scale", obj.tile_scale);
		tileShader->setMat4("model", obj.modelMatrix);

		if (!obj.textures || obj.textures->empty()) {
			obj.model->Draw(*tileShader);
		}
		else {
			obj.model->Draw(*tileShader, *obj.textures);
		}

	}

	animShader->use();
	animShader->setInt("is_light", 0);
	
	// Animated Objects
	animShader->setInt("is_animating", 1);
	for (const auto& obj : animatedObjects) {
		
		animShader->setMat4("model", obj.modelMatrix);
		glm::vec4 color = glm::vec4(obj.color);
		animShader->setVec4("color", color);

		if (!obj.animator->final_bone_matrices.empty()) {
			animShader->setMat4Array("finalBonesMatrices", obj.animator->final_bone_matrices);
		}
		
		if (!obj.textures || obj.textures->empty()) {
			obj.model->Draw(*animShader);
		}
		else {
			obj.model->Draw(*animShader, *obj.textures);
		}

	}

	// Static Objects
	animShader->setInt("is_animating", 0);
	for (const auto& obj : staticObjects) {

		animShader->setMat4("model", obj.modelMatrix);
		glm::vec4 color = glm::vec4(obj.color);
		animShader->setVec4("color", color);

		if (!obj.textures || obj.textures->empty()) {
			obj.model->Draw(*animShader);
		}
		else {
			obj.model->Draw(*animShader, *obj.textures);
		}

	}

}

void RenderSystem::renderShadows()
{

	Shader* shadowShader = ResourceManager::Instance().shader_shadow;

	shadowShader->use();
	shadowShader->setInt("is_animating", 1);

	for (const auto& obj : animatedObjects) {

		shadowShader->setMat4("model", obj.modelMatrix);
		
		if (!obj.animator->final_bone_matrices.empty()) {
			shadowShader->setMat4Array("finalBonesMatrices", obj.animator->final_bone_matrices);
		}

		obj.model->Draw(*shadowShader);

	}

	shadowShader->setInt("is_animating", 0);
	for (const auto& obj : staticObjects) {
		shadowShader->setMat4("model", obj.modelMatrix);
		obj.model->Draw(*shadowShader);
	}

	for (const auto& obj : tileObjects) {
		shadowShader->setMat4("model", obj.modelMatrix);
		obj.model->Draw(*shadowShader);
	}

}

void RenderSystem::clear()
{
	animatedObjects.clear();
	staticObjects.clear();
	tileObjects.clear();
}
