#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCord;
//layout (location = 3) in vec3 aColor;
//layout (location = 7) in float aOffset;
layout (location = 8) in mat4 aModel;

out vec3 Position;
out vec2 TextureCoords;
out vec3 Normal;
out vec4 Color;
//out float Scale;

uniform float totalTime;
uniform float scaleFactor;

uniform mat4 view;
uniform mat4 projection;

void main() {

	float time = totalTime;

	float alpha = clamp(time, 0.0, 1.0);
	Color = vec4(vec3(1.f), 1.f);

	//float scale = scaleFactor * (sin(time + aOffset) + 2.f) / 3.f;

	TextureCoords = aTexCord;
	Normal = aNormal;

	//float scaleX = length(vec3(model[0]));
    //float scaleY = length(vec3(model[1]));
    //float scaleZ = length(vec3(model[2]));

	//vec3 scaledPos = aPos * scale;
	//Position = vec3(aModel * vec4(scaledPos, 1.0));
	gl_Position = projection * view * aModel * vec4(aPos, 1.0);

}
