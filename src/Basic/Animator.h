#pragma once
#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Animation.h"

#define MAX_BONES 100

class Animator
{
public:
	Animation* current_animation, *next_animation;
	std::vector<glm::mat4> final_bone_matrices;
	float current_time;
	float delta_time;
	float blend_time;
	float blend_duration;

	int begin = 0, end = 0;

	bool repeat = true, blending = false, ended = false, into_next = false;

	Animator(Animation* animation_to_play) {
		current_time = 0.f;
		current_animation = animation_to_play;
		next_animation = nullptr;
		final_bone_matrices.reserve(MAX_BONES);

		for (int i = 0; i < MAX_BONES; i++)
			final_bone_matrices.push_back(glm::mat4(1.0f));
	}

	void playAnimation(Animation* animation, bool repeat = true);
	void blendAnimation(Animation* next_animation, float duration, bool into_next = false, bool repeat = false);
	bool isPlayingNonLooping() const;
	void updateAnimation(float delta_time);
	void calculateBoneTransform(Ass_impNodeData* node, glm::mat4 parent_transform);
	void blendAnimations(Animation* A, Animation* B, float alpha);
	void blendBoneTransform(Animation* A, Animation* B, Ass_impNodeData* nodeA, Ass_impNodeData* nodeB, glm::mat4 parentA, glm::mat4 parentB, float alpha);
};





#endif // !ANIMATOR_H


