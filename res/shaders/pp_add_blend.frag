#version 330 core

in vec2 vectorUV;

uniform sampler2D baseTexture;
uniform sampler2D lightScatteringTexture;

out vec4 FragColor;

void main()
{
    vec4 base = texture(baseTexture, vectorUV);
    vec4 light = texture(lightScatteringTexture, vectorUV);

    // Additive blend
    FragColor = base + light;
}
