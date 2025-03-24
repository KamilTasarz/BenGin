#version 330 core

in VS_OUT {
	vec3 Pos;
	vec2 Cords;
	vec3 Normal;
	//mat3 TBN;
} fs_in;

out vec4 FragColor;

struct PointLight {

	vec3 position, ambient, diffuse, specular;
	float constant, linear, quadratic;

};

struct Material {
    sampler2D diffuse_map;
    sampler2D specular_map;
    //sampler2D normal_map;
    float shininess;
};

uniform PointLight light;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float shininess;
uniform vec3 cameraPosition;
uniform int is_light;

void main() {

	if (is_light != 1) {
    
        // obtain normal from normal map in range [0,1]
        //vec3 normal = texture(normal_map, fs_in.TexCoords).rgb;
        // transform normal vector to range [-1,1]
        //normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

	    vec3 lightDir = normalize(light.position - fs_in.Pos);
        vec3 viewDir = normalize(cameraPosition - fs_in.Pos);
        float diff = max(dot(fs_in.Normal, lightDir), 0.0);

        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(fs_in.Normal, halfwayDir), 0.0), shininess);

        float distance = distance(light.position, fs_in.Pos);
        float attenuation = 1.0f / (light.quadratic * distance * distance + light.linear * distance + light.constant);

        vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, fs_in.Cords));
        vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, fs_in.Cords));
        vec3 specular = light.specular * spec * vec3(texture(texture_specular1, fs_in.Cords));
        FragColor = vec4((ambient + diffuse + specular) * attenuation, 1.f);

    } else {
        FragColor = vec4(1.f);
    }

}