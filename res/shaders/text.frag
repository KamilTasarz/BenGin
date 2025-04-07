#version 330 core

in vec2 Cords;

out vec4 FragColor;

uniform vec3 text_color;
uniform sampler2D letter;

void main() {

	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(letter, Cords).r); //określa przezroczystość 
	FragColor = vec4(text_color, 1.f) * sampled; //zmiania kolor na właściwy jak jest odpowiednia wartość alfa

}