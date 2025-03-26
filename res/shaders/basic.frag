#version 460 core

in vec3 Normal;
in vec3 Pos;
in vec2 Cords;
in vec4 Light_Perspective_Pos;
in vec4 Light_Perspective_Pos2;
in vec4 Light_Perspective_Pos3;

out vec4 FragColor;

// struktury

struct PointLight {

	vec3 position, ambient, diffuse, specular;
	float constant, linear, quadratic;

};

struct DirectionLight {

	vec3 direction, ambient, diffuse, specular;
	
};

// definicje metod

mat3 calculatePointLight(vec3 viewDir, PointLight light);
mat3 calculateDirectionalLight(vec3 viewDir, DirectionLight light);
float calulateShadow(vec4 position_from_light_perpective, sampler2D map);

// uniformy

uniform int is_light;
uniform int point_light_number;
uniform int directional_light_number;

uniform PointLight point_lights[10];
uniform DirectionLight directional_lights[10];

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D shadow_map;
uniform sampler2D shadow_map3;
uniform sampler2D shadow_map_back;

uniform vec3 cameraPosition;

float shininess = 64.f;


void main() {

	if (is_light != 1) {
        
        vec3 viewDir = normalize(cameraPosition - Pos);
	    mat3 res = mat3(0.f);
        for (int i = 0; i < point_light_number; i++) {
            res += calculatePointLight(viewDir, point_lights[i]);
        }
        for (int i = 0; i < directional_light_number; i++) {
            res += calculateDirectionalLight(viewDir, directional_lights[i]);
        }

        float shadow1 = min(calulateShadow(Light_Perspective_Pos, shadow_map), calulateShadow(Light_Perspective_Pos2, shadow_map_back));

        float shadow2 = calulateShadow(Light_Perspective_Pos3, shadow_map3);

        float shadow = max(shadow1, shadow2) * 0.5f;

        FragColor = vec4((res[0] + (res[1] + res[2]) * (1.f - shadow)), 1.f);

    } else {
        FragColor = vec4(1.f);
    }

}

// implementacje metod

mat3 calculatePointLight(vec3 viewDir, PointLight light) {
    vec3 lightDir = normalize(light.position - Pos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), shininess);

    float distance = distance(light.position, Pos);
    float attenuation = 1.0f / (light.quadratic * distance * distance + light.linear * distance + light.constant);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, Cords)) * attenuation;
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, Cords)) * attenuation;
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, Cords)) * attenuation;

    return mat3(ambient, diffuse, specular);
}

mat3 calculateDirectionalLight(vec3 viewDir, DirectionLight light) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), shininess);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, Cords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, Cords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, Cords));

    return mat3(ambient, diffuse, specular);
}

float calulateShadow(vec4 position_from_light_perpective, sampler2D map) {
    vec3 position = position_from_light_perpective.xyz / position_from_light_perpective.w;
    position = position * 0.5f + 0.5f;
    
    float current_depth = position.z;
    float bias = 0.005f;

    float shadow = 0.f;
    vec2 pixel_size = 1.f / textureSize(map, 0);

    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            vec2 offset = vec2(x, y) * pixel_size;
            float closest_depth = texture(map, position.xy + offset).r;
            if (current_depth - bias > closest_depth) shadow += 1.0f;
        }
    }
    shadow /= 25.0f; // Dla 5x5 kernelu

    return shadow;
}