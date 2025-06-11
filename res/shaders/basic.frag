#version 330 core

in VS_OUT {
	vec3 Pos;
	vec2 Cords;
	vec3 Normal;
	vec4 Light_Perspective_Pos;
	//mat3 TBN;
} fs_in;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragNormal;

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
float calculateShadow(vec4 position_from_light_perpective, sampler2D map);
float calculateShadowPointLight(PointLight p, samplerCube map);

// uniformy

uniform int is_light;
uniform int point_light_number;
uniform int directional_light_number;

uniform sampler2D shadow_map1;
uniform samplerCube shadow_maps[16];
uniform PointLight point_lights[16];
uniform DirectionLight directional_lights[1];

uniform int useShadows;
uniform float far_plane;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;


uniform vec3 cameraPosition;

float shininess = 64.f;


void main() {

    vec3 N = normalize(fs_in.Normal);
    FragNormal = vec4(N * 0.5 + 0.5, 1.0);  

	if (is_light != 1) {
    
        // obtain normal from normal map in range [0,1]
        //vec3 normal = texture(normal_map, fs_in.TexCoords).rgb;
        // transform normal vector to range [-1,1]
        //normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

        
        vec3 viewDir = normalize(cameraPosition - fs_in.Pos);
	    mat3 res = mat3(0.f);
        for (int i = 0; i < point_light_number; i++) {
            mat3 p = calculatePointLight(viewDir, point_lights[i]);
            float visibility = calculateShadowPointLight(point_lights[i], shadow_maps[i]);
            res[0] += p[0]; // ambient
            res[1] += p[1] * (1-visibility); // diffuse
            res[2] += p[2] * (1-visibility); // specular
        }
        for (int i = 0; i < directional_light_number; i++) {
            res += calculateDirectionalLight(viewDir, directional_lights[i]);
        }



        //shadow = shadow / float(point_light_number);

        vec3 finalColor = (res[0] + (res[1] + res[2]));

        FragColor = vec4(finalColor, 1.0);
    } else {
        FragColor = vec4(1.f);
    }

}

// implementacje metod

mat3 calculatePointLight(vec3 viewDir, PointLight light) {
    vec3 lightDir = normalize(light.position - fs_in.Pos);
    float diff = max(dot(fs_in.Normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(fs_in.Normal, halfwayDir), 0.0), shininess);

    float _distance = distance(light.position, fs_in.Pos);
    float attenuation = 1.0f / (light.quadratic * _distance * _distance + light.linear * _distance + light.constant);


    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, fs_in.Cords)) * attenuation;
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, fs_in.Cords)) * attenuation;
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, fs_in.Cords)) * attenuation;

    return mat3(ambient, diffuse, specular);
}

mat3 calculateDirectionalLight(vec3 viewDir, DirectionLight light) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(fs_in.Normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(fs_in.Normal, halfwayDir), 0.0), shininess);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, fs_in.Cords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, fs_in.Cords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, fs_in.Cords));

    return mat3(ambient, diffuse, specular);
}

float calculateShadow(vec4 position_from_light_perpective, sampler2D map) {
    
    if (useShadows == 0) return 1.f;

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
    shadow /= 25.0f; 

    return shadow;
}

float calculateShadowPointLight(PointLight p, samplerCube map)
{
    //if (useShadows == 0) return 0.0;

    vec3 fragToLight = (fs_in.Pos - p.position);
    float currentDepth = length(fragToLight);
    fragToLight = normalize(fragToLight);

    float closestDepth = texture(map, fragToLight).r * far_plane;

    float bias = 0.05f;
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

    return shadow;
}