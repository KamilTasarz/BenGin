#version 330 core
layout (location = 0) in vec4 aPos;

out vec2 Cords;

uniform mat4 projection;


void main()
{
	gl_Position = projection * vec4(aPos.xy, 0.0f, 1.0f);

	Cords = aPos.zw;
}