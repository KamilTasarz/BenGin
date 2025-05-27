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
	// SSAO noise texture
	GLuint noise_texture;

	SceneGraph* sceneGraph;
	PostProcessData postProcessData;
	unsigned int framebuffer, colorTexture, depthTexture, normalTexture, depthRenderbuffer;
	unsigned int ssaoFBO, ssaoColorBuffer, ssaoBlurFBO, ssaoBlurColorBuffer;

	Animator* animator;
	Animation* anim;

	void input();
	void draw();
	void update(float deltaTime);

public:

	bool play = true;
	Game(std::vector<std::shared_ptr<Prefab>>& prefabsref, std::vector<std::shared_ptr<Prefab>>& prefabsref_puzzle);

	std::vector<glm::vec3> updateSSAOKernel(unsigned int samples);
	void generateNoiseTexture();
	void debug_print();

	void init();
	void run();
	void shutdown();

};

#endif // !GAME_H
