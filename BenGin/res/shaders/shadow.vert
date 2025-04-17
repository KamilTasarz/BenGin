#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 5) in ivec4 aBoneId;
layout (location = 6) in vec4 aWeight;

uniform mat4 view_projection;
uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
	vec4 pos = vec4(0.f);
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
		if (aBoneId[i] < 0) {
			//if (i == MAX_BONE_INFLUENCE - 1) pos = vec4(aPos, 1.f);
			continue;
		}
		if (aBoneId[i] >= MAX_BONES) {
			pos = vec4(aPos, 1.f);
			break;
		}
		vec4 localPosition = finalBonesMatrices[aBoneId[i]] * vec4(aPos,1.0f);
        pos += localPosition * aWeight[i];
		
	}
	if (length(pos) == 0.f) {
		pos = vec4(aPos, 1.f);
	}

	gl_Position = view_projection * model * pos;
};