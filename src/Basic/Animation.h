#pragma once

#ifndef ANIMATION_H
#define ANIMATION_H

#include <assimp/Importer.hpp>

#include "Bone.h"
#include "Model.h"

struct Ass_impNodeData {
	glm::mat4 transform;
	std::string name;
	int children_count;
	std::vector<Ass_impNodeData> children;
};

class Animation {
public:
	float duration = 0.f;
	float speed = 0.f;
	std::vector<Bone> bones;
	Ass_impNodeData root;
	std::map<std::string, BoneInfo> m_BoneInfoMap;

	Animation() = default;

	Animation(const char* animation_path, Model &model) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animation_path, aiProcess_Triangulate);
		if (scene && scene->mRootNode) {
			cout << "Wczytano" << endl;
			aiAnimation *anim = scene->mAnimations[0];
			duration = anim->mDuration;
			speed = anim->mTicksPerSecond;
			readHierarchy(root, scene->mRootNode);
			ReadMissingBones(anim, model);
		}
	}

	Bone* FindBone(const std::string& name)
	{
		auto iter = std::find_if(bones.begin(), bones.end(),
			[&](const Bone& Bone)
			{
				return Bone.getBoneName() == name;
			}
		);
		if (iter == bones.end()) return nullptr;
		else return &(*iter);
	}

	void readHierarchy(Ass_impNodeData& destination, aiNode* source);
	// if anim load without some bones add function to fix this
	void ReadMissingBones(const aiAnimation* animation, Model& model);
};

#endif // !ANIMATION_H
