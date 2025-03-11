#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCord;
//layout (location = 3) in mat4 model;

out vec3 Normal;
out vec2 Cords;
out vec3 Pos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;



void main()
{
	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	Pos = vec3(model * vec4(aPos, 1.0));
	Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
	Cords = aTexCord;
};