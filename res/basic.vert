#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 3) in mat4 model;

out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;



void main()
{
	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	Normal = mat3(transpose(inverse(model))) * aNormal;
};