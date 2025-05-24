#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCord;
//layout (location = 3) in vec3 aColor;
layout (location = 7) in float aOffset_time;
layout (location = 8) in vec3 aOffset;

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


	float pulse = 1.0 + scaleFactor * sin(aOffset_time + totalTime / 10.f);
	vec3 local = aPos * pulse;
	vec4 worldPos = vec4(local + aOffset, 1.0);

	//vec3 scaledPos = aPos * scale;
	//Position = vec3(aModel * vec4(scaledPos, 1.0));
	gl_Position = projection * view * worldPos;

}
