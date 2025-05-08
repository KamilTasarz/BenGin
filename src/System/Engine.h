#pragma once

#ifndef ENGINE_H
#define ENGINE_H

#include "Editor.h"

class Window;

class Engine {
private:
	std::vector<std::shared_ptr<Prefab>> prefabs;

public:
	Editor* editor;
	Window* window;

	Engine() = default;
	~Engine() = default;

	void init();
	void run();
	void shutdown();

};

#endif // !ENGINE_H

