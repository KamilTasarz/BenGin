#version 330 core

in vec2 vectorUV;

out float FragColor;

uniform sampler2D gDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise; // losowe obracanie sampli (bez efektu bandingu)

const int MAX_SAMPLES = 128;
uniform vec3 samples[MAX_SAMPLES];
uniform int kernelSize;

uniform mat4 projection; // view -> clip space
uniform mat4 invProjection; // clip -> view space
uniform vec2 screenSize; 

uniform float radius; // promien hemisfery
uniform float bias; // delikatne przesuniecie - redukcja artefaktow
uniform float intensity; // sila efektu
uniform vec2 noise_scale; // czestotliwosc powtarzania sie efektu

vec3 getViewPos(vec2 uv, float depth);

void main() {

    float depth = texture(gDepth, vectorUV).r; // Get depth from texture
    //FragColor = depth;
    vec3 P      = getViewPos(vectorUV, depth); // Oblicz pozycje z glebokosci, stad bierzemy probki hemisfery

    vec3 N = normalize(texture(gNormal, vectorUV).rgb * 2.0 - 1.0); // Get normal from texture and normalize

    // Get vector from noise texture
    vec3 randomVec = normalize(
        texture(texNoise, vectorUV * noise_scale/*screenSize / 4.0*/).xyz
    );

    // Oblicz macierz TBN
    vec3 tangent   = normalize(randomVec - N * dot(randomVec, N));
    vec3 bitangent = cross(N, tangent);
    mat3 TBN       = mat3(tangent, bitangent, N);

    // Calculate occlusion from all the samples
    float occlusion = 0.0;
    for (int i = 0; i < kernelSize; ++i) {

        vec3 sampleVS = TBN * samples[i];       // sample from hemisphere to view-space
        sampleVS      = P + sampleVS * radius;  // przesunięcie od P

        // go back to tex coords
        vec4 offset = projection * vec4(sampleVS, 1.0); // pomnoz przez projection (view -> clip space)
        offset.xyz /= offset.w;
        vec2 sampleUV = offset.xy * 0.5 + 0.5;

        // odczytaj głębokość w punkcie próbki
        float sampleDepth = texture(gDepth, sampleUV).r;
        vec3 samplePos = getViewPos(sampleUV, sampleDepth); // oblicz pozycje probki z glebokosci

        // obliczamy wspolczynnik wygladzajacy
        float rangeCheck = smoothstep(
            0.0, 1.0, radius / abs(P.z - samplePos.z)
        );

        if (samplePos.z >= sampleVS.z + bias)
            occlusion += rangeCheck; // sprawdzamy czy probka jest glebiej niz to dla czego sprawdzamy i jak tak to dodajemy

    }

    // Get final occlusion color
    FragColor = 1.0 - (occlusion / float(kernelSize)) * intensity;
}

vec3 getViewPos(vec2 uv, float depth) {

    float z = depth * 2.0 - 1.0;
    vec4 clip = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 view = invProjection * clip;
    return view.xyz / view.w;

}
