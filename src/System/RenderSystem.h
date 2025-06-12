#pragma once

#include "../config.h"

class Model;
class Animator;
class Node;
class SceneGraph;

struct RenderObject {
	// Define the properties of a renderable object
	// For example, it could include a mesh, material, transform, etc.
	std::shared_ptr<Model> model;
	glm::mat4 modelMatrix;
	Animator* animator;
	float tile_scale = 1.f;
	glm::vec4 color = { 1.f, 1.f, 1.f, 1.f }; // Default color is white
};

class RenderSystem
{
private:
	std::vector<RenderObject> animatedObjects;
	std::vector<RenderObject> staticObjects;
	std::vector<RenderObject> tileObjects;

public:

	static RenderSystem& Instance() {
		static RenderSystem instance;
		return instance;
	}

	void addAnimatedObject(Node* obj);
	void addStaticObject(Node* obj);
	void addTileObject(Node* obj);

	void render();
	void renderShadows();
	void clear();
};

