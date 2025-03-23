#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCord;
//layout (location = 3) in mat4 model;

out vec3 Normal;
out vec2 Cords;
out vec3 Pos;
out vec4 Light_Perspective_Pos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 light_view_projection;



void main()
{
	
	Pos = vec3(model * vec4(aPos, 1.0));
	gl_Position = projection * view * vec4(Pos, 1.0f);

	Light_Perspective_Pos = light_view_projection * vec4(Pos, 1.0f);

	Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
	
	//pobranie skali i określenie, czy jest jednorodna, czy nie i ustalenie wspolrzednych tekstury do wyswietlenia
	vec3 scale = vec3(length(model[0].xyz), length(model[1].xyz), length(model[2].xyz));
	if (scale.x == scale.y && scale.x == scale.z) scale = vec3(1.f);

	if (Normal.y != 0.f) 
		Cords = aTexCord * scale.xz;
	else if (Normal.x != 0.f)
		Cords = aTexCord * scale.yz;
	else 
		Cords = aTexCord * scale.xy;

	
};