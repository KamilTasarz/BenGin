#version 330 core
out vec4 FragColor;
in vec2 vectorUV;

uniform sampler2D image;
uniform bool horizontal;
uniform vec2 screenSize;

void main() {
    float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
    vec2 texelSize = 1.0 / screenSize;

    vec3 result = texture(image, vectorUV).rgb * weights[0];

    for (int i = 1; i < 5; ++i) {
        vec2 offset = horizontal ? vec2(texelSize.x * i, 0.0) : vec2(0.0, texelSize.y * i);
        result += texture(image, vectorUV + offset).rgb * weights[i];
        result += texture(image, vectorUV - offset).rgb * weights[i];
    }

    FragColor = vec4(result, 1.0);
}
