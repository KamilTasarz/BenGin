#version 330 core

in vec2 vectorUV;

out float FragColor;

uniform sampler2D gDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

const int MAX_SAMPLES = 128;
uniform vec3 samples[MAX_SAMPLES];
uniform int kernelSize;

uniform mat4 projection;
uniform mat4 invProjection;
uniform float radius;
uniform vec2 screenSize;

vec3 getViewPos(vec2 uv, float depth);

void main() {

    // 1) Pobierz głębokość i zrekonstruuj P
    float depth = texture(gDepth, vectorUV).r;
    //FragColor = depth;
    vec3 P      = getViewPos(vectorUV, depth);

    // 2) Pobierz i zdekoduj normalę
    vec3 N = normalize(texture(gNormal, vectorUV).rgb * 2.0 - 1.0);

    // 3) Lokalny losowy wektor z noise + TBN
    vec3 randomVec = normalize(
        texture(texNoise, vectorUV * screenSize / 4.0).xyz
    );
    vec3 tangent   = normalize(randomVec - N * dot(randomVec, N));
    vec3 bitangent = cross(N, tangent);
    mat3 TBN       = mat3(tangent, bitangent, N);

    // 4) SSAO: pętla po próbkach
    float occlusion = 0.0;
    for (int i = 0; i < kernelSize; ++i) {
        vec3 sampleVS = TBN * samples[i];       // obróć do view-space
        sampleVS      = P + sampleVS * radius;  // przesunięcie od P

        // projektuj z powrotem do NDC → UV
        vec4 offset = projection * vec4(sampleVS, 1.0);
        offset.xyz /= offset.w;
        vec2 sampleUV = offset.xy * 0.5 + 0.5;

        // odczytaj głębokość w punkcie próbki
        float sampleDepth = texture(gDepth, sampleUV).r;
        vec3 samplePos = getViewPos(sampleUV, sampleDepth);

        // range check i akumulacja
        float rangeCheck = smoothstep(
            0.0, 1.0, radius / abs(P.z - samplePos.z)
        );
        if (samplePos.z >= sampleVS.z)
            occlusion += rangeCheck;
    }

    // 5) finalna wartość AO
    FragColor = 1.0 - (occlusion / float(kernelSize));
}

vec3 getViewPos(vec2 uv, float depth) {

    float z = depth * 2.0 - 1.0;
    vec4 clip = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 view = invProjection * clip;
    return view.xyz / view.w;

}
