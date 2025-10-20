#include "Animation.h"
#include "Model.h"

Animation::Animation(const char* animation_path, Model& model, int num) {
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
			//cout << "Wczytano animacje: " << animation_path << " " << num << endl;
		}
	}
}

Animation::Animation(aiAnimation* anim, Model& model, const aiScene* scene) {
	if (anim) {
		duration = anim->mDuration;
		speed = anim->mTicksPerSecond;
		name = anim->mName.C_Str();
		readHierarchy(root, scene->mRootNode);
		ReadMissingBones(anim, model);

	}
}

void Animation::readHierarchy(Ass_impNodeData& destination, aiNode* source)
{
	if (source) {
		destination.name = source->mName.data;
		destination.children_count = source->mNumChildren;
		aiMatrix4x4 mat4 = source->mTransformation;
		destination.transform = glm::transpose(glm::mat4(glm::vec4(mat4.a1, mat4.a2, mat4.a3, mat4.a4), glm::vec4(mat4.b1, mat4.b2, mat4.b3, mat4.b4),
			glm::vec4(mat4.c1, mat4.c2, mat4.c3, mat4.c4), glm::vec4(mat4.d1, mat4.d2, mat4.d3, mat4.d4)));
		for (int i = 0; i < source->mNumChildren; i++) {
			Ass_impNodeData child;
			readHierarchy(child, source->mChildren[i]);
			destination.children.push_back(child);
		}
	}
}

void Animation::ReadMissingBones(const aiAnimation* animation, Model& model)
{
	int size = animation->mNumChannels;

	auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
	int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

	//reading channels(bones engaged in an animation and their keyframes)
	for (int i = 0; i < size; i++)
	{
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = boneCount;
			boneCount++;
		}
		bones.push_back(Bone(channel, boneInfoMap[channel->mNodeName.data].id, std::string(channel->mNodeName.data)));
	}

	m_BoneInfoMap = boneInfoMap;
}