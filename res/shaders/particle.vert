#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 instancePosition;
layout (location = 3) in vec4 instanceColor;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    float scale = 0.008f;
    vec3 worldPos = instancePosition + aPosition * scale;

    TexCoords = aTexCoords;
    ParticleColor = instanceColor;
    gl_Position = projection * view * vec4(worldPos, 1.0);
}