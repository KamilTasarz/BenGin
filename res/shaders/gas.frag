#version 330 core

in vec3 PositionF;
in vec2 TextureCoordsF;
in vec3 NormalF;
in vec4 ColorF;

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

float fractalNoise(vec2 uv, float t) {
    float n = 0.0;
    n += 0.7 * noise(uv * 2.0 + vec2(t * 0.2, t * 0.1));
    n += 0.2 * noise(uv * 6.0 + vec2(t * 0.4, -t * 0.3));
    n += 0.1 * noise(uv * 12.0 - vec2(t * 0.7, t * 0.6));
    return n;
}

void main() {
    
    //vec3 N = normalize(Normal);
    //FragNormal = vec4(N * 0.5 + 0.5, 1.0);

    //FragColor = Color;

    vec2 uv = TextureCoordsF;
    
    vec2 drift = vec2(sin(time * 0.3), cos(time * 0.2)) * 0.05;
    vec2 swirl = vec2(uv.y, -uv.x) * 0.03 * sin(time * 0.4);
    vec2 uvFlow = uv + drift + swirl;


    // Dynamiczny szum
    //float n = noise(uvFlow * 6.0 + time * 0.5);
    float n = fractalNoise(uvFlow, time);
    float alpha = smoothstep(0.3, 0.8, n);
    
    // Fade na brzegach quada (opcjonalnie)
    float edge = smoothstep(0.5, 0.0, length(uv - 0.5));
    alpha *= edge;

    // float edgeFade = pow(1.0 - length(uv - 0.5) * 1.5, 2.0);
    // alpha *= clamp(edgeFade, 0.0, 1.0);

    vec3 base = vec3(0.2, 1.0, 0.4);
    vec3 alt  = vec3(0.0, 0.3, 0.0); // ciemniejszy zielony
    vec3 gasColor = mix(alt, base, n); // zale¿noœæ od szumu


    //vec3 gasColor = vec3(0.2, 1.0, 0.4); // seledynowy zielony

    FragColor = vec4(gasColor, alpha * ColorF.a);

    if (FragColor.a < 0.01)
        discard;

}