#version 330 core
out vec4 FragColor;

in vec2 vectorUV;

uniform sampler2D sceneColor;  // kolor z g³ównego renderingu
uniform sampler2D ssaoMap;     // rozmyta mapa SSAO

void main() {

    vec3 color = texture(sceneColor, vectorUV).rgb;
    float ao = texture(ssaoMap, vectorUV).r;
    
    // AO zaciemnia obraz
    color *= ao;

    FragColor = vec4(color, 1.0);

}
