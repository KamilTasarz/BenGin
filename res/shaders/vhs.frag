#version 330 core

in vec2 vectorUV;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform vec2 curvature; // jak mocno jest widoczny efekt zakrzywienia
uniform float time;

// Szum oparty o pozycję i czas
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

//vec2 

void main()
{
    float offset = 0.005;
    float glitch = sin(vectorUV.y * 50.0 + time * 5.0) * 0.02;

    float r = texture(screenTexture, vec2(vectorUV.x + offset + glitch, vectorUV.y)).r;
    float g = texture(screenTexture, vectorUV).g;
    float b = texture(screenTexture, vec2(vectorUV.x - offset - glitch, vectorUV.y)).b;
    vec3 color = vec3(r, g, b);

    float noise = random(vectorUV * 100.0 + time) * 0.15;
    float line = sin(vectorUV.y * 600.0 + time * 20.0) * 0.03;

    color += vec3(noise + line);
    color = clamp(color, 0.0, 1.0);

    FragColor = vec4(color, 1.0);
    //FragColor = texture(screenTexture, vectorUV);

}
