#version 460 core

in vec3 Normal;
in vec3 Pos;
in vec2 Cords;

out vec4 FragColor;

struct PointLight {

	vec3 position, ambient, diffuse, specular;
	float constant, linear, quadratic;

};

struct Material {
    sampler2D diffuse_map;
    sampler2D specular_map;
    float shininess;
};

uniform PointLight light;
uniform Material material;
uniform vec3 cameraPosition;
uniform int isLight;

void main() {

	if (isLight != 1) {
    
	    vec3 lightDir = normalize(light.position - Pos);
        vec3 viewDir = normalize(cameraPosition - Pos);
        float diff = max(dot(Normal, lightDir), 0.0);

        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), material.shininess);

        float distance = distance(light.position, Pos);
        float attenuation = 1.0f / (light.quadratic * distance * distance + light.linear * distance + light.constant);

        vec3 ambient = light.ambient * vec3(texture(material.diffuse_map, Cords));
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse_map, Cords));
        vec3 specular = light.specular * spec * vec3(texture(material.specular_map, Cords));
        FragColor = vec4((ambient + diffuse + specular) * attenuation, 1.f);

    } else {
        FragColor = vec4(1.f);
    }

}