#version 330 core

in vec2 vectorUV;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform bool is_rewind;

void main() {

	vec4 original = texture(screenTexture, vectorUV);

	if (is_rewind) {
		FragColor = original + vec4(0.0, 0.5, 0.0, 1.0);
	} else
		FragColor = original + vec4(0.5, 0.0, 0.0, 1.0);

}