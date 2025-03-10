#version 460 core

in vec3 Normal;

out vec4 FragColor;

struct PointLight {

	vec3 position, ambient, diffuse, specular;
	float constant, linear, quadratic;

};

uniform PointLight light;

void main() {
	

	FragColor = vec4(0.2f, 0.2f, 0.8f, 1.0f);
}