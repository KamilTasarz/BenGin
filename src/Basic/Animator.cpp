#include "Animator.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

void Animator::playAnimation(Animation* animation, bool repeat)
{
	this->repeat = repeat;

	begin = 1;
	end = 0;

    ended = false;
    blending = false;
	current_animation = animation;
	current_time = 0.0f;
}

void Animator::blendAnimation(Animation* next_animation, float duration, bool into_next, bool repeat)
{
    this->repeat = repeat;
    blending = true;
    ended = false;
    this->into_next = into_next;
    begin = 1;
    end = 0;
    blend_duration = duration;
    
    blend_time = 0.0f;
    this->next_animation = next_animation;
}

bool Animator::isPlayingNonLooping() const {
    return !repeat && !ended;
}

void Animator::updateAnimation(float delta_time)
{
    this->delta_time = delta_time;

    if (begin == end && !repeat) ended = true;

    if (!blending) {
        if (current_animation) {
            if (begin == end && (begin == 0 || repeat)) begin++;

            if (begin != end) {

                if (current_time + current_animation->speed * delta_time > current_animation->duration) {
                    end++;

                    if (repeat) {
                        current_time += current_animation->speed * delta_time;
                        current_time = fmodf(current_time, current_animation->duration);
                    }
                    return;
                }
                else {
                    current_time += current_animation->speed * delta_time;
                }

                current_time = fmodf(current_time, current_animation->duration);
                calculateBoneTransform(&current_animation->root, glm::mat4(1.f));
            }
        }
    }
    else {
        if (next_animation) {

            blend_time += next_animation->speed * delta_time;

            float alpha = 0.0f;
            if (blend_duration > 0.f) {
                alpha = (float)blend_time / (float)blend_duration;
                if (blend_time > blend_duration) alpha = 1.f;
            }

            //cout << alpha << endl;

            if (alpha < 1.f) {

                blendAnimations(current_animation, next_animation, alpha);
            }
            else {
                current_animation = next_animation;
                next_animation = nullptr;
                blending = false;
                current_time = 0.0f;
                if ((into_next && blend_duration > current_animation->duration) && !repeat) ended = true;

                if (into_next) current_time = blend_time;
                //if (!repeat) ended = true;
            }
        }
        else {
            blending = false;
        }
    }
}


void Animator::calculateBoneTransform(Ass_impNodeData* node, glm::mat4 parent_transform)
{
	std::string name = node->name;
	glm::mat4 transform = node->transform;//node->transform;

	Bone* bone = current_animation->FindBone(name);

	//assert(bone != nullptr);

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

void Animator::blendAnimations(Animation* A, Animation* B, float alpha)
{

    
	glm::mat4 identity = glm::mat4(1.0f);
	blendBoneTransform(A, B, &A->root, &B->root, identity, identity, alpha);
}

void Animator::blendBoneTransform(Animation* A, Animation* B, Ass_impNodeData* nodeA, Ass_impNodeData* nodeB, glm::mat4 parentA, glm::mat4 parentB, float alpha)
{
    std::string name = nodeA->name;
    Bone* boneA = A->FindBone(name);
    Bone* boneB = B->FindBone(name);

    glm::mat4 localA = nodeA->transform;
    glm::mat4 localB = nodeB->transform;

    // Decompose both matrices
    glm::vec3 posA, scaleA, posB, scaleB;
    glm::quat rotA, rotB;
    glm::vec3 skew; glm::vec4 perspective;

    if (boneA) {

        boneA->update(current_time);

        localA = boneA->local_model_matrix;
    }
    

    if (boneB) {
        //boneB->update(current_time);

        float timeB = 0.f;
        if (into_next) {
            timeB = fmodf(blend_duration, B->duration);
        }
        boneB->update(timeB);

        localB = boneB->local_model_matrix;
    }

    glm::mat4 globalA = parentA * localA;
    glm::mat4 globalB = parentB * localB;

    glm::decompose(globalA, scaleA, rotA, posA, skew, perspective);
    glm::decompose(globalB, scaleB, rotB, posB, skew, perspective);
    
    // Interpolate
    glm::vec3 final_pos = glm::mix(posA, posB, alpha);
    glm::quat final_rot = glm::slerp(rotA, rotB, alpha);
    glm::vec3 final_scale = glm::mix(scaleA, scaleB, alpha);
    
    glm::mat4 blended = glm::translate(glm::mat4(1.0f), final_pos) *
        glm::mat4_cast(final_rot) *
        glm::scale(glm::mat4(1.0f), final_scale);

    
    
    glm::mat4 global_blended = blended;



    // Save to final bone matrices
    if (A->m_BoneInfoMap.find(name) != A->m_BoneInfoMap.end()) {
        int index = A->m_BoneInfoMap[name].id;

        glm::mat4 offset = A->m_BoneInfoMap[name].offset;
        final_bone_matrices[index] = global_blended * offset;
    }

    for (int i = 0; i < nodeA->children_count; ++i) {
        blendBoneTransform(A, B, &nodeA->children[i], &nodeB->children[i], globalA, globalB, alpha);
    }
}


