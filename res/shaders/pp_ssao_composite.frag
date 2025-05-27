#version 330 core
out vec4 FragColor;

in vec2 vectorUV;

uniform sampler2D sceneColor;  // kolor z 
uniform sampler2D ssaoMap;     // rozmyta mapa SSAO
uniform float aoPower;     // power

void main() {

    vec3 color = texture(sceneColor, vectorUV).rgb;
    float ao = texture(ssaoMap, vectorUV).r;
    
    ao = pow(ao, aoPower);

    // AO zaciemnia obraz
    color *= ao;

    FragColor = vec4(color, 1.0);

}
