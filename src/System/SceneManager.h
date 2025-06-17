#pragma once
#include "../config.h"

class SceneManager
{
public:
	SceneManager() = default;
	~SceneManager() = default;
	void Initialize();
	void Update(float deltaTime);
	void Render();
	void LoadScene(const std::string& sceneName);
	void UnloadScene(const std::string& sceneName);

};

