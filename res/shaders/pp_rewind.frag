#version 330 core

in vec2 vectorUV;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform bool is_rewind;

uniform float time;

uniform float noise_alpha;
uniform float band_speed;
uniform float num_bands;
uniform float band_thickness;

float random(vec2 uv) {
    return fract(sin(dot(uv.xy ,vec2(12.9898,78.233))) * 43758.5453123);
}

float simpleNoise(vec2 uv) {
    vec2 i = floor(uv);
    vec2 f = fract(uv);
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    vec2 u = f*f*(3.0-2.0*f);
    return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

void main() {

    vec2 uv = vectorUV;

	vec4 original = texture(screenTexture, uv);

	if (!is_rewind) {
		FragColor = original;
        return;
    }

    // 1. Noise for the whole screen
    vec2 noiseUV = uv + vec2(mod(time * 117.0, 113.0));
    float noiseVal = simpleNoise(noiseUV * 200.0);

    // 2. Scrolling bands
    float scrollY = mod(uv.y + time * band_speed, 1.0);
    float bandStep = 1.0 / num_bands;
    float band = mod(scrollY, bandStep);

    float bandMask = step(0.0, band_thickness - band);

    float bandNoise = simpleNoise(vec2(uv.x * 50.0, uv.y * 50.0 + time * 10.0));
    vec4 bandColor = vec4(vec3(bandNoise), 1.0);

    // 3. UV distortion
    float rippleFreq = 20.0;
    float rippleAmp = 0.005;
    float ripple = sin((uv.y + time * 0.8) * rippleFreq) * rippleAmp;
    uv.x += ripple;

    vec4 distorted = texture(screenTexture, uv);

    vec4 combined = mix(distorted, bandColor, bandMask);

    FragColor = mix(combined, vec4(vec3(noiseVal), 1.0), noise_alpha);

}