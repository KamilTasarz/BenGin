#pragma once

#include "Script.h"

class TextObject;
class SpriteObject;
class GameManagerRewindable;

class UIManager : public Script
{
public:
	TextObject* scoreText;
	TextObject* deathCountText;
	TextObject* runTimeText;
	TextObject* fpsText;
	TextObject* stateText;
	SpriteObject* playSprite;
	SpriteObject* rewindSprite;
	TextObject* timeline;
	TextObject* timelineTop;
	TextObject* timelineDown;

	GameManagerRewindable* rewindable = nullptr;
	int startHistorySize = 0;
	bool historyEmpty = true;
	bool isRewinding = false;

	float runTime = 0.f;
	int deathCount = 0;
	float score = 0.f;

	float fpsAccumulator = 0.f;
	int frameCount = 0;
	float timeSinceLastUpdate = 0.f;
	const float updateInterval = 0.5f;

	UIManager() = default;
	~UIManager() = default;
	void onAttach(Node* owner) override;
	void onDetach() override;
	void onStart() override;
	void onUpdate(float deltaTime) override;
	void HandleRewindTimeline();
};
