#include "Animator.h"

void Animator::playAnimation(Animation* animation)
{
	current_animation = animation;
	current_time = 0.0f;
}

void Animator::updateAnimation(float delta_time)
{
	this->delta_time = delta_time;
	if (current_animation) {
		current_time += current_animation->speed * delta_time;
		current_time = fmodf(current_time, current_animation->duration);
		calculateBoneTransform(&current_animation->root, glm::mat4(1.f));
	}
}


void Animator::calculateBoneTransform(Ass_impNodeData* node, glm::mat4 parent_transform)
{
	std::string name = node->name;
	glm::mat4 transform = node->transform;//node->transform;

	Bone* bone = current_animation->FindBone(name);

	if (bone) {
		bone->update(current_time);
		transform = bone->local_model_matrix;
	}

	glm::mat4 global_model_matrix = parent_transform * transform;

	

	if (current_animation->m_BoneInfoMap.find(name) != current_animation->m_BoneInfoMap.end()) {
		int index = current_animation->m_BoneInfoMap[name].id;
		glm::mat4 offset = current_animation->m_BoneInfoMap[name].offset;
		final_bone_matrices[index] = global_model_matrix * offset;
	}

	for (int i = 0; i < node->children_count; i++) {
		calculateBoneTransform(&node->children[i], global_model_matrix);
	}
}
