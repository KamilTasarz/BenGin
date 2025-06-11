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

