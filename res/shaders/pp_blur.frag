#version 330 core
out vec4 FragColor;
in vec2 vectorUV;

uniform sampler2D image;
uniform bool horizontal;
uniform vec2 screenSize;

uniform int bloom_blur_radius;
uniform float bloom_blur_sigma;
uniform float bloom_blur_intensity;

void main() {
    
    //float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    vec2 texelSize = 1.0 / screenSize;

    float weights[20];
    float sum = 0.0;
    for (int i = 0; i <= bloom_blur_radius; ++i) {
        float w = exp(-float(i*i) / (2.0 * bloom_blur_sigma * bloom_blur_sigma));
        weights[i] = w;
        sum += (i == 0) ? w : w * 2.0;
    }

    vec3 result = vec3(0.0);
    for (int i = 0; i <= bloom_blur_radius; ++i) {
        float w = weights[i] / sum;           // znormalizowana waga
        vec2 offset = horizontal
            ? vec2(texelSize.x * i, 0.0)
            : vec2(0.0, texelSize.y * i);
        
        vec3 c = texture(image, vectorUV + offset).rgb;
        result += c * w;
        if (i > 0) {
            vec3 c2 = texture(image, vectorUV - offset).rgb;
            result += c2 * w;
        }
    }

    result *= bloom_blur_intensity;

    FragColor = vec4(result, 1.0);

}
