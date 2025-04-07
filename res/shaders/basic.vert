#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCord;
//layout (location = 3) in vec2 aTangent;
//layout (location = 4) in vec2 aBitangent;

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
//uniform mat4 light_view_projection3;
//uniform mat4 light_view_projection_back;


void main()
{

	//vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
    //vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
    //vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
    //vs_out.TBN = mat3(T, B, N);

	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	vs_out.Pos = vec3(model * vec4(aPos, 1.0));
	vs_out.Normal = normalize(mat3(transpose(inverse(model))) * aNormal);

	vs_out.Light_Perspective_Pos = light_view_projection * vec4(vs_out.Pos, 1.0f);
	//vs_out.Light_Perspective_Pos2 = light_view_projection_back * vec4(vs_out.Pos, 1.0f);
	//vs_out.Light_Perspective_Pos3 = light_view_projection3 * vec4(vs_out.Pos, 1.0f);

	
	vec3 scale = vec3(length(model[0].xyz), length(model[1].xyz), length(model[2].xyz));
	if (scale.x == scale.y && scale.x == scale.z) scale = vec3(1.f);


	if (vs_out.Normal.y != 0.f) 
		vs_out.Cords = aTexCord * scale.xz;
	else if (vs_out.Normal.x != 0.f)
		vs_out.Cords = aTexCord * scale.yz;
	else 
		vs_out.Cords = aTexCord * scale.xy;

	
}
