#version 330 core

in vec2 vectorUV;

out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform float bloom_threshold;

void main()
{
    vec3 color = texture(sceneTexture, vectorUV).rgb;

    // Luminancja (prosty model)
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

    // Zatrzymaj tylko jasne fragmenty
    //FragColor = vec4(vec3(brightness), 1.0);  // debuguj, jak jasna jest scena

    FragColor = brightness > bloom_threshold ? vec4(color, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
}
