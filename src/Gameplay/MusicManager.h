#pragma once

#include <glm/ext/vector_float2.hpp>

class MusicManager
{
public:
	int menuId = -1;
	int baseId = -1;
	int transitionId = -1;
	int rewindId = -1;

	bool menuActive = false;
	bool pauseActive = false;
	bool transitionActive = false;
	bool rewindActive = false;

	int currentStage = 1;
	float menuVolume = 0.f;
	float stageVolumes[4] = { 0.f, 0.f, 0.f, 0.f };
	int stageChannels[4] = { -1, -1, -1, -1 };

	float beatTime = 1.92f;
	float timer = 0.f;
	float overflownBeatTime = 0.f;

	float volume = 80.f;

	static MusicManager& instance();
	void Init();
	void Update(float deltaTime);
	void StartGameTransition();
	void StartGameMusic();
	int getTargetStage(float distance, bool isInGas);
	void PlayRewindSound();
	void StopRewindSound();
	void onEnd();
};
