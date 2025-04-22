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

uniform vec3 rimColor = vec3(1.0, .1, .1); // biały kontur
uniform float rimPower = 8.0;               // im większy, tym ostrzejsza krawędź

out vec4 FragColor;

void main()
{
    vec3 viewDir = normalize(cameraPosition - fs_in.Pos);
    vec3 norm = normalize(fs_in.Normal);

    // Rim light oparty na kącie między normalą a widokiem
    float rim = 1.0 - max(dot(viewDir, norm), 0.0);
    rim = pow(rim, rimPower); // ostrzejsze przejście

    // Finalny kolor – czarny obiekt z białym konturem
    vec3 finalColor = vec3(0.0) + rim * rimColor;

    FragColor = vec4(finalColor, 1.0);
}
