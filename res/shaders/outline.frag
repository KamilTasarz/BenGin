#version 460 core

layout(location = 0) out vec4 FragColor;
//layout(location = 1) out vec4 FragNormal;

uniform vec3 color;

void main() {

    //FragNormal = vec4(1.0);

    FragColor = vec4(color, 0.7f);  

}