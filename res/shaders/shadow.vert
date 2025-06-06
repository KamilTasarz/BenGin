#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 5) in ivec4 aBoneId;
layout (location = 6) in vec4 aWeight;

uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
uniform int is_animating;
void main()
{
	vec4 pos = vec4(0.f);
	if (is_animating == 1) {
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
			if (aBoneId[i] < 0 || aWeight[i] == 0.0) continue;
			vec4 localPosition = finalBonesMatrices[aBoneId[i]] * vec4(aPos,1.0f);
			pos += localPosition * aWeight[i];
		}
	}
	
	if (is_animating != 1) {
		pos = vec4(aPos, 1.0);
	}

	gl_Position = model * pos;

};