#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCord;
//layout (location = 3) in vec3 aColor;
layout (location = 7) in float beginTime;
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
	
	if (beginTime > 0.f) { 

		float life_time = 20.f;

		float time = totalTime - beginTime;

		float alpha = clamp(time, 0.0, 1.0);
		Color = vec4(vec3(1.f), 1.f);

		float scale_base = 1.f;

		if (time < 3.f) scale_base = time / 3.f;
		else if (time > 17.f) scale_base = ((life_time - time) / 3.f);

		float pulse = 1.f;

		if (time >= 3.f && time <= 17.f) {

			pulse = scale_base + scaleFactor * sin(beginTime + time);
		} else {
			pulse = scale_base;
		}
		vec3 local = aPos * pulse;
		vec4 worldPos = vec4(local + aOffset, 1.0);

		//vec3 scaledPos = aPos * scale;
		//Position = vec3(aModel * vec4(scaledPos, 1.0));
		gl_Position = projection * view * worldPos;
	} else {
		gl_Position = vec4(0.f);
	}

}
