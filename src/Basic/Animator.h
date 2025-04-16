#pragma once
#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Animation.h"

#define MAX_BONES 100

class Animator
{
public:
	Animation* current_animation;
	std::vector<glm::mat4> final_bone_matrices;
	float current_time;
	float delta_time;
	Animator(Animation* animation_to_play) {
		current_time = 0.f;
		current_animation = animation_to_play;
		final_bone_matrices.reserve(MAX_BONES);

		for (int i = 0; i < MAX_BONES; i++)
			final_bone_matrices.push_back(glm::mat4(1.0f));
	}

	void playAnimation(Animation* animation);
	void updateAnimation(float delta_time);
	void calculateBoneTransform(Ass_impNodeData* node, glm::mat4 parent_transform);

};





#endif // !ANIMATOR_H


