#pragma once

#ifndef ANIMATION_H
#define ANIMATION_H

#include <assimp/Importer.hpp>

#include <map>

#include "Bone.h"

class Model;

struct Ass_impNodeData {
	glm::mat4 transform;
	std::string name;
	int children_count;
	std::vector<Ass_impNodeData> children;
};

struct BoneInfo
{
	// id wykorzystywane w shaderze do okreslenia z macierzy kosci, ktora to jest macierz
	int id;

	// offset przenoszacy kosc z originu
	glm::mat4 offset;

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

	Animation(const char* animation_path, Model& model, int num = 0);

	Animation(aiAnimation* anim, Model& model, const aiScene* scene);

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
