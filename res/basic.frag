#version 460 core

in vec3 Normal;

out vec4 FragColor;

struct PointLight {

	vec3 pos, col;

	float a, b, c;
};

uniform PointLight light;

void main() {
	

	FragColor = vec4(0.2f, 0.2f, 0.8f, 1.0f);
}