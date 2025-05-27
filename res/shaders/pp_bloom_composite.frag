#version 330 core
out vec4 FragColor;
in vec2 vectorUV;

uniform sampler2D sceneTexture;
uniform sampler2D bloomTexture;
uniform float intensity; // np. 1.0

void main() {
    vec3 scene = texture(sceneTexture, vectorUV).rgb;
    vec3 bloom = texture(bloomTexture, vectorUV).rgb;

    vec3 hdr = scene + bloom;
    vec3 mapped = vec3(1.0) - exp(-hdr * intensity);

    FragColor = vec4(mapped, 1.0);
}
