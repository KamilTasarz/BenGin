#pragma once

#ifndef ENGINE_H
#define ENGINE_H

#include "Game.h"

class Window;
class CAudioEngine;

class Engine {
private:
	std::vector<std::shared_ptr<Prefab>> prefabs;
	std::vector<std::shared_ptr<Prefab>> prefabs_puzzle;
	
public:
	Game* game;
	Window* window;
	CAudioEngine* audio;

	Engine() = default;
	~Engine() = default;

	void init();
	void run();
	void shutdown();

};

#endif // !ENGINE_H

