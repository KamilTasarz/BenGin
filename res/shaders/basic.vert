#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCord;
//layout (location = 3) in vec2 aTangent;
//layout (location = 4) in vec2 aBitangent;
layout (location = 5) in ivec4 aBoneId;
layout (location = 6) in vec4 aWeight;

out VS_OUT {
	vec3 Pos;
	vec2 Cords;
	vec3 Normal;
	vec4 Light_Perspective_Pos;
	vec4 Light_Perspective_Pos2;
	vec4 Light_Perspective_Pos3;
	//mat3 TBN;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 light_view_projection;
uniform mat4 light_view_projection3;
uniform mat4 light_view_projection_back;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{

	//vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
    //vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
    //vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
    //vs_out.TBN = mat3(T, B, N);
	vec4 pos = vec4(0.f);
	vec3 localNormal = vec3(0.0);
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
		localNormal += mat3(finalBonesMatrices[aBoneId[i]]) * aNormal * aWeight[i];
	}
	


	if (length(pos) == 0.0) {
		pos = vec4(aPos, 1.0);
	}

	gl_Position = projection * view * model * pos;
	vs_out.Pos = vec3(model * pos);
	if (length(localNormal) == 0.0f) {
		vs_out.Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
	} else {
		vs_out.Normal = normalize(mat3(transpose(inverse(model))) * localNormal);
	}

	vs_out.Light_Perspective_Pos = light_view_projection * vec4(vs_out.Pos, 1.0f);
	vs_out.Light_Perspective_Pos2 = light_view_projection_back * vec4(vs_out.Pos, 1.0f);
	vs_out.Light_Perspective_Pos3 = light_view_projection3 * vec4(vs_out.Pos, 1.0f);

	
	vec3 scale = vec3(length(model[0].xyz), length(model[1].xyz), length(model[2].xyz));

if (abs(scale.x - scale.y) < 0.001 && abs(scale.x - scale.z) < 0.001) {
    vs_out.Cords = aTexCord;
} else {
    if (abs(vs_out.Normal.y) > 0.9) {
        vs_out.Cords = aTexCord * scale.xz;
    } else if (abs(vs_out.Normal.x) > 0.9) {
        vs_out.Cords = aTexCord * scale.yz;
    } else if (abs(vs_out.Normal.z) > 0.9) {
        vs_out.Cords = aTexCord * scale.xy;
    } 
}

	//float scaleFinal = max(scale.x, max(scale.y, scale.z));
	//vs_out.Cords = aTexCord;
	
}
