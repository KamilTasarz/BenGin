#version 330 core

in VS_OUT {
	vec3 Pos;
	vec2 Cords;
	vec3 Normal;
	vec4 Light_Perspective_Pos;
	//vec4 Light_Perspective_Pos2;
	//vec4 Light_Perspective_Pos3;
	//mat3 TBN;
} fs_in;

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

uniform PointLight point_lights[100];
uniform DirectionLight directional_lights[10];

uniform int useShadows;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D shadow_map1;
//uniform sampler2D shadow_map3;
//uniform sampler2D shadow_map_back;

uniform vec3 cameraPosition;

float shininess = 64.f;


void main() {

	if (is_light != 1) {
    
        // obtain normal from normal map in range [0,1]
        //vec3 normal = texture(normal_map, fs_in.TexCoords).rgb;
        // transform normal vector to range [-1,1]
        //normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

        
        vec3 viewDir = normalize(cameraPosition - fs_in.Pos);
	    mat3 res = mat3(0.f);
        for (int i = 0; i < point_light_number; i++) {
            res += calculatePointLight(viewDir, point_lights[i]);
        }
        for (int i = 0; i < directional_light_number; i++) {
            res += calculateDirectionalLight(viewDir, directional_lights[i]);
        }

        

        float shadow = calulateShadow(fs_in.Light_Perspective_Pos, shadow_map1);
        //float shadow1 = 0.f;//min(calulateShadow(fs_in.Light_Perspective_Pos, shadow_map), calulateShadow(fs_in.Light_Perspective_Pos2, shadow_map_back));
        
        //float shadow = clamp(shadow1+shadow2, 0.f, 1.f);


        FragColor = vec4((res[0] + (res[1] + res[2]) * (1.f - shadow)), 1.f);

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

    float distance = distance(light.position, fs_in.Pos);
    float attenuation = 1.0f / (light.quadratic * distance * distance + light.linear * distance + light.constant);

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

float calulateShadow(vec4 position_from_light_perpective, sampler2D map) {
    
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