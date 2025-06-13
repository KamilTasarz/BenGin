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

		float instancePhase = length(aOffset.xy) * 10.0; 

		float life_time = 40.f;
		float time = totalTime - (beginTime + instancePhase);

		Color = vec4(
			0.2f,
			0.9f - 0.1f * pow(sin((beginTime / 3.14f + time / 4.f)), 2),
			0.2f,
			0.6f + 0.4f * pow(sin((beginTime + time / 2.f)), 2)
		);

		float scale_base = 1.0f;

		// Płynne pojawianie się i znikanie
		if (time < 3.f) {
			scale_base = smoothstep(0.0, 3.0, time);
		} else if (time > life_time - 3.f) {
			scale_base = smoothstep(life_time, life_time - 3.0, time);
		}

		// Pulsowanie po fazie pojawiania się
		float pulse = scale_base;

		

		if (time >= 3.f && time <= life_time - 3.f) {
			float localTime = time - 3.0; // startuje od 0 dla płynności
			pulse += 0.15f * sin(localTime * 2.0 + instancePhase);
		}

		vec3 local = aPos * pulse;
		vec4 worldPos = vec4(local + aOffset, 1.0);
		gl_Position = projection * view * worldPos;

	} else {
		gl_Position = vec4(0.f);
	}
}

