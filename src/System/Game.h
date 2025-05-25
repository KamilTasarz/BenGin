#pragma once
#ifndef GAME_H
#define GAME_H

#include "../config.h"

#include "PostProcessData.h"

class Prefab;
class SceneGraph;
class BoundingBox;

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

	SceneGraph* sceneGraph;
	PostProcessData postProcessData;
	unsigned int framebuffer, colorTexture, normalTexture, depthRenderbuffer;

	void input();
	void draw();
	void update(float deltaTime);
public:
	bool play = true;
	Game(std::vector<std::shared_ptr<Prefab>>& prefabsref);

	void init();
	void run();
	void shutdown();
};

#endif // !GAME_H
