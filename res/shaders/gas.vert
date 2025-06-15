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

		float life_time = 40.0;


		float time = totalTime - beginTime;

		float alpha = clamp(time, 0.0, 1.0);
		Color = vec4(0.2f, 0.9f - 0.1f * pow(sin((beginTime / 3.14f + time / 4.f)), 2), 0.2f, 0.6f + 0.4 * pow(sin((beginTime + time / 2.f)), 2));

		float scale_base = 0.9;

		if (time < 3.0) scale_base = 0.9 * time / (3.0);
		else if (time > life_time - (3.0)) scale_base = 0.9 * ((life_time - time) / (3.0 ));

		float pulse = 1.f;

		

		if (time >= (3.0) && time <= life_time - (3.0)) {
			time = time - 3.0; // przesunięcie czasu o 3 sekundy
			pulse = scale_base + 0.15 * sin(time * 2.0);
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
