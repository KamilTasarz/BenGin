#version 330 core
out float FragColor;
in vec2 vectorUV;

uniform sampler2D ssaoInput;
uniform vec2 screenSize;

void main() {

    float result = 0.0;
    vec2 texelSize = 1.0 / screenSize;

    // 3x3 kernel
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, vectorUV + offset).r;
        }
    }

    result /= 9.0;
    FragColor = result;

}
