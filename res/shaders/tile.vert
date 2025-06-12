#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCord;
//layout (location = 3) in vec2 aTangent;
//layout (location = 4) in vec2 aBitangent;
layout (location = 5) in ivec4 aBoneId;
layout (location = 6) in vec4 aWeight;

out VS_OUT {
	vec3 Pos;
	vec2 Cords;
	vec3 Normal;
	vec4 Light_Perspective_Pos;
	//vec4 Light_Perspective_Pos2;
	//vec4 Light_Perspective_Pos3;
	//mat3 TBN;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 light_view_projection;
uniform float tile_scale;

void main()
{

	gl_Position = projection * view * model * vec4(aPos, 1.f);
	vs_out.Pos = vec3(model * vec4(aPos, 1.f));
	
	vs_out.Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
	

	vs_out.Light_Perspective_Pos = light_view_projection * vec4(vs_out.Pos, 1.0f);

	
	vec3 scale = vec3(length(model[0].xyz), length(model[1].xyz), length(model[2].xyz));

	if (abs(aNormal.z) > 0.9) {
		vs_out.Cords =  -aTexCord * scale.xy;
	} else if (abs(aNormal.y) > 0.9) {
		vs_out.Cords =  aTexCord * scale.xz;
	} else {
		vs_out.Cords = aTexCord * scale.yz ;
	}
	
}
