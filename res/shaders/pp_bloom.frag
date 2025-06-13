#version 330 core

in vec2 vectorUV;

out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform float bloom_threshold;
uniform float bloom_smoothness;

void main()
{
    vec3 color = texture(sceneTexture, vectorUV).rgb;

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

    //FragColor = vec4(vec3(brightness), 1.0);

    float down_scale = bloom_threshold - bloom_smoothness; 
    float up_scale = bloom_threshold + bloom_smoothness; 

    float factor = smoothstep(down_scale, up_scale, brightness);

    FragColor = brightness > bloom_threshold ? vec4(color * factor, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);

    //FragColor = vec4(color * factor, 1.0);

}
