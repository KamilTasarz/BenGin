#version 330 core
out vec4 FragColor;
in vec2 vectorUV;

uniform sampler2D gNormal;

void main() {
    // Mapuj z [-1,1] do [0,1], by móc wyœwietliæ na ekranie
    vec3 normal = texture(gNormal, vectorUV).rgb;
    normal = normalize(normal) * 0.5 + 0.5;
    FragColor = vec4(normal, 1.0);
}
