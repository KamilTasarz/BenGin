#version 330 core

in vec2 Cords;

out vec4 FragColor;

uniform sampler2D background;

void main() {

	
	vec4 final = texture(background, Cords);
	if (final.w < 0.1f) discard;
	FragColor = final;
	//else FragColor.w = 0.5f;

}