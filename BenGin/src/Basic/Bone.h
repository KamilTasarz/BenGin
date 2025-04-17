#pragma once
#ifndef BONE_H
#define BONE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <vector>



struct KeyTranslation {
	glm::vec3 translation;
	float time_stamp;
};

struct KeyRotation {
	glm::quat orientation;
	float time_stamp;
};

struct KeyScale {
	glm::vec3 scale;
	float time_stamp;
};

class Bone {
public:
	std::string m_name;
	std::vector<KeyTranslation> translation_keys;
	std::vector<KeyRotation> rotation_keys;
	std::vector<KeyScale> scale_keys;

	int num_position_keys, num_rotation_keys, num_scale_keys;

	int id;
	glm::mat4 local_model_matrix;

	Bone(aiNodeAnim* anim_channel, int ID, std::string name) : m_name(name), id(ID) {
		local_model_matrix = glm::mat4(1.f);

		num_position_keys = anim_channel->mNumPositionKeys;
		for (int i = 0; i < num_position_keys; i++) {
			KeyTranslation key_translation;
			key_translation.time_stamp = anim_channel->mPositionKeys[i].mTime;
			aiVector3D temp_translation = anim_channel->mPositionKeys[i].mValue;
			key_translation.translation = glm::vec3(temp_translation.x, temp_translation.y, temp_translation.z);
			translation_keys.push_back(key_translation);
		}

		num_rotation_keys = anim_channel->mNumRotationKeys;
		for (int i = 0; i < num_rotation_keys; i++) {
			KeyRotation key_rotation;
			key_rotation.time_stamp = anim_channel->mRotationKeys[i].mTime;
			aiQuaternion temp_rotation = anim_channel->mRotationKeys[i].mValue;
			key_rotation.orientation = glm::quat(temp_rotation.w, temp_rotation.x, temp_rotation.y, temp_rotation.z);
			rotation_keys.push_back(key_rotation);
		}

		num_scale_keys = anim_channel->mNumScalingKeys;
		for (int i = 0; i < num_scale_keys; i++) {
			KeyScale key_scale;
			key_scale.time_stamp = anim_channel->mScalingKeys[i].mTime;
			aiVector3D temp_scale = anim_channel->mScalingKeys[i].mValue;
			key_scale.scale = glm::vec3(temp_scale.x, temp_scale.y, temp_scale.z);
			scale_keys.push_back(key_scale);
		}
	}

	glm::mat4 interpolatePosition(float animation_time);
	glm::mat4 interpolateRotation(float animation_time);
	glm::mat4 interpolateScale(float animation_time);
	float getScaleFactor(float animation_time, float start_time, float end_time);
	int getPositionIndex(float animation_time);
	int getRotationIndex(float animation_time);
	int getScaleIndex(float animation_time);
	void update(float animation_time);
	std::string getBoneName() const { return m_name; }
};

#endif // !BONE_H
