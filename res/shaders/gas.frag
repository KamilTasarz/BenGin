#version 330 core

in vec3 Position;
in vec2 TextureCoords;
in vec3 Normal;
in vec4 Color;

out vec4 FragColor;

uniform float time;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

void main() {
    
    //vec3 N = normalize(Normal);
    //FragNormal = vec4(N * 0.5 + 0.5, 1.0);

    //FragColor = Color;

    //if (FragColor.a < 0.01)
        //discard;

    vec2 uv = TextureCoords;
    
    // Dynamiczny szum
    float n = noise(uv * 6.0 + time * 0.5);
    float alpha = smoothstep(0.4, 0.8, n);
    
    // Fade na brzegach quada (opcjonalnie)
    float edge = smoothstep(0.4, 0.0, length(uv - 0.5));
    alpha *= edge;

    vec3 gasColor = vec3(0.2, 1.0, 0.4); // seledynowy zielony

    FragColor = vec4(gasColor, alpha * Color.a);

    if (FragColor.a < 0.01)
        discard;

}