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
	std::string name;
	std::vector<Bone> bones;
	Ass_impNodeData root;
	std::map<std::string, BoneInfo> m_BoneInfoMap;

	Animation() = default;

	Animation(const char* animation_path, Model& model, int num = 0) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animation_path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		if (scene && scene->mRootNode) {

			if (num < scene->mNumAnimations) {
				aiAnimation* anim = scene->mAnimations[num];
				duration = anim->mDuration;
				speed = anim->mTicksPerSecond;
				name = anim->mName.C_Str();
				readHierarchy(root, scene->mRootNode);
				ReadMissingBones(anim, model);
				cout << "Wczytano animacje: " << animation_path << " " << num << endl;
			}
		}
	}

	Animation(aiAnimation* anim, Model& model, const aiScene* scene) {
		if (anim) {
			duration = anim->mDuration;
			speed = anim->mTicksPerSecond;
			name = anim->mName.C_Str();
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
