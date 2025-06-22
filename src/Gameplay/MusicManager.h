#pragma once

#include <glm/ext/vector_float2.hpp>

class MusicManager
{
public:
	int menuId = -1;
	int stageOneId = -1;
	int stageTwoId = -1;
	int stageThreeId = -1;
	int stageFourId = -1;

	bool menuActive = true;
	bool stageOneActive = false;
	bool stageTwoActive = false;
	bool stageThreeActive = false;
	bool stageFourActive = false;

	float stageTwoVolume = 0.f;
	float stageThreeVolume = 0.f;
	float stageFourVolume = 0.f;

	glm::vec2 distanceToPlayer;

	void Init();
	void Update(float deltaTime);
	void StartGameMusic();
	void onEnd();
};
