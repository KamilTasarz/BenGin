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

struct DirectionLight {

	vec3 direction, ambient, diffuse, specular;
	
};

uniform vec3 cameraPosition;
uniform vec3 lightColor;

uniform DirectionLight directional_light[1];

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform vec3 rimColor = vec3(1.0, 0.0, 0.0); 
uniform float rimPower = 8.0;               

out vec4 FragColor;

void main()
{
    vec3 viewDir = normalize(cameraPosition - fs_in.Pos);
    vec3 norm = normalize(fs_in.Normal);

    float rim = 1.0 - max(dot(viewDir, norm), 0.0);
    rim = pow(rim, rimPower); 
	float rimIntensity = 1.f;
   
    vec3 finalColor = vec3(0.f) + rim * rimColor * rimIntensity;
	float alpha = 1.f;
	if (length(finalColor) < 0.5f) { alpha = 0.f; };

    FragColor = vec4(finalColor, alpha);
}
