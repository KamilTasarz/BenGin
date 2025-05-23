#pragma once
#ifndef GAME_H
#define GAME_H

#include "../config.h"

class Prefab;
class SceneGraph;
class BoundingBox;
class Animator;
class Animation;


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

	SceneGraph* sceneGraph;
	unsigned int framebuffer, colorTexture, depthRenderbuffer;

	Animator* animator;
	Animation* anim;

	void input();
	void draw();
	void update(float deltaTime);
public:
	bool play = true;
	Game(std::vector<std::shared_ptr<Prefab>>& prefabsref, std::vector<std::shared_ptr<Prefab>>& prefabsref_puzzle);

	void init();
	void run();
	void shutdown();
};

#endif // !GAME_H




