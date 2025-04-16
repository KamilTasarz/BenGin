#version 330 core

in vec2 Cords;

out vec4 FragColor;

uniform sampler2D background;

void main() {

	
	FragColor = texture(background, Cords);
	//if (FragColor.w < 0.1f) discard;
	//else FragColor.w = 0.5f;

}