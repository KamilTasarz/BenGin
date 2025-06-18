#version 330 core

in vec2 vectorUV;
out vec4 FragColor;

uniform sampler2D depthTexture;

uniform vec3 lightPos;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float lightRadius;
uniform float lightConeAngle; // in radians
uniform float lightIntensity;

uniform mat4 invViewProj;

uniform vec3 camPos;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * 0.1 * 50.0) / (50.0 + 0.1 - z * (50.0 - 0.1));
}

vec3 reconstructWorldPos(vec2 uv, float depth) {
    vec4 clip = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 viewPos = invViewProj * clip;
    viewPos /= viewPos.w;
    return viewPos.xyz;
}

void main() {

    float depth = texture(depthTexture, vectorUV).r;
    float linearDepth = LinearizeDepth(depth);

    vec3 worldPos = reconstructWorldPos(vectorUV, depth);

    vec3 rayDir = normalize(worldPos - camPos);
    vec3 rayOrigin = camPos;

    float t = 0.0;
    float stepSize = lightRadius / 64.0;
    float contribution = 0.0;

    for (int i = 0; i < 64; ++i) {
        vec3 samplePos = rayOrigin + rayDir * t;
        float dist = length(samplePos - lightPos);
        if (dist > lightRadius)
            break;

        vec3 toSample = normalize(samplePos - lightPos);
        float angle = acos(dot(toSample, normalize(lightDir)));
        if (angle > lightConeAngle)
            break;

        float falloff = 1.0 - dist / lightRadius;
        float coneAtten = 1.0 - angle / lightConeAngle;
        contribution += falloff * coneAtten * stepSize;

        t += stepSize;
    }

    vec3 color = lightColor * contribution * lightIntensity;
    FragColor = vec4(color, 1.0);
    //FragColor = vec4(vec3(linearDepth / 50.0), 1.0);

}
