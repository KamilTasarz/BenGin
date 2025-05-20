#version 330 core

in vec3 Position;
in vec2 TextureCoords;
in vec3 Normal;
in vec4 Color;

out vec4 FragColor;

void main() {
    
    FragColor = Color;

    if (FragColor.a < 0.01)
        discard;

}