#version 330 core

in vec3 Position;
in vec2 TextureCoords;
in vec3 Normal;
in vec4 Color;

layout(location = 0) out vec4 FragColor;
//layout(location = 1) out vec4 FragNormal;

void main() {
    
    //vec3 N = normalize(Normal);
    //FragNormal = vec4(N * 0.5 + 0.5, 1.0);

    FragColor = Color;

    if (FragColor.a < 0.01)
        discard;

}