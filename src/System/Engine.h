#pragma once

#ifndef ENGINE_H
#define ENGINE_H

#include "Editor.h"
#include "Game.h"

class Window;

class Engine {
private:
	std::vector<std::shared_ptr<Prefab>> prefabs;
	std::vector<std::shared_ptr<Prefab>> prefabs_puzzle;
	
public:
	Editor* editor;
	Game* game;
	Window* window;

	Engine() = default;
	~Engine() = default;

	void init();
	void run();
	void shutdown();

};

#endif // !ENGINE_H

