#version 330 core
out vec4 FragColor;
in vec2 vectorUV;

uniform sampler2D image;
uniform float alpha;
uniform float time;
uniform vec2 resolution;

float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec2 uv = vectorUV.xy / resolution.xy;

    float noise = rand(uv * time * 60.0);

    noise += rand(vec2(uv.y * 100.0, time * 5.0)) * 0.3;

    noise *= step(0.2, fract(sin(time * 10.0) * 43758.5453));
    vec3 finalColor = mix(texture(image, vectorUV.xy).rgb, vec3(noise), 1 - alpha);
    gl_FragColor = vec4(finalColor, 1.f);
}
