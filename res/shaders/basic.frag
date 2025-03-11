#version 460 core

in vec3 Normal;
in vec3 Pos;
in vec2 Cords;

out vec4 FragColor;

struct PointLight {

	vec3 position, ambient, diffuse, specular;
	float constant, linear, quadratic;

};

uniform PointLight light;
uniform sampler2D ourTexture;
uniform vec3 cameraPosition;

void main() {
	
	//swiatlo
	vec3 ambient = light.ambient;

	vec3 lightDir = normalize(light.position - Pos);

	

	FragColor = texture(ourTexture, Cords);//vec4(0.2f, 0.2f, 0.8f, 1.0f);
}