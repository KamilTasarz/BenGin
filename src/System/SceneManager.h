#pragma once
#include "../config.h"

class SceneGraph;

struct OrderedScene
{
	std::string path_name;
	int order;
};

class SceneManager
{
private:
	std::vector<OrderedScene> scenes;
	int currentSceneIndex = -1;
	SceneGraph* sceneGraph = nullptr;

public:
	SceneManager() = default;
	~SceneManager();

	SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}

	//load next scene
	void next();

	//load previous scene
	void previous();

	//load current index scene again
	void reset();

	//get pointer to current scene graph
	SceneGraph* getCurrentScene();

	void Initialize();

};

