#version 330 core
out vec4 FragColor;
in vec2 vectorUV;

uniform sampler2D ssaoMap;

void main() {

    float ao = texture(ssaoMap, vectorUV).r;
    FragColor = vec4(vec3(ao), 1.0);

}
