#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCord;
layout (location = 3) in vec3 aColor;
layout (location = 4) in float aOffset;
layout (location = 5) in mat4 aModel;

out vec3 Position;
out vec2 TextureCoords;
out vec3 Normal;
out vec4 Color;
out float Scale;

uniform float totalTime;
uniform float scaleFactor;

uniform mat4 view;
uniform mat4 projection;

void main() {

	totalTime /= 100.0f;

	float alpha = clamp(totalTime, 0.0f, 1,0f);
	Color = vec4(aColor, alpha);

	Scale = scaleFactor * sin(totalTime + offset);
	TextureCoords = aTexCord;
	Normal = aNormal;

	Position = vec3(aModel * vec4(aPos, 1.0));
	gl_Position = projection * view * aModel * vec4(aPos, 1.0);

}
