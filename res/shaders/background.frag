#version 330 core

in vec2 Cords;

out vec4 FragColor;

uniform sampler2D background;

void main() {

	
	FragColor = texture(background, Cords);

}