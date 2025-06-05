#pragma once
#ifndef GAME_H
#define GAME_H

#include "../config.h"

#include "PostProcessData.h"

class Prefab;
class SceneGraph;
class BoundingBox;
class Animator;
class Animation;
struct Ray;

class Game
{

private:

	bool is_initialized = false;

	int viewX;
	int viewY;
	int viewWidth;
	int viewHeight;

	GLuint quadVAO, quadVBO;

	std::vector<BoundingBox*> colliders;
	std::vector<std::shared_ptr<Prefab>>& prefabs;
	std::vector<std::shared_ptr<Prefab>>& puzzle_prefabs;

	// SSAO vectors
	std::vector<glm::vec3> ssao_kernel;

	SceneGraph* sceneGraph;
	PostProcessData postProcessData;
	unsigned int framebuffer, colorTexture, normalTexture, depthRenderbuffer;

	glm::vec2 normalizedMouse;

	Animator* animator;
	Animation* anim;

	void input();
	void draw();
	void update(float deltaTime);

public:

	bool play = true;
	Game(std::vector<std::shared_ptr<Prefab>>& prefabsref, std::vector<std::shared_ptr<Prefab>>& prefabsref_puzzle);

	Ray getRayWorld(GLFWwindow* window, const glm::mat4& _view, const glm::mat4& _projection);

	void init();
	void run();
	void shutdown();

};

#endif // !GAME_H
