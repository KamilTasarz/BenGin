#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform float u_time;

out vec4 FragColor; // albo in/out jak masz dalej

// prosta funkcja hashująca — da pseudo losowy szum
float hash(vec3 p) {
    return fract(sin(dot(p ,vec3(12.9898,78.233, 37.719))) * 43758.5453);
}

void main() {
    for (int i = 0; i < 3; ++i) {
        vec4 pos = gl_in[i].gl_Position;

        vec3 offsetDir = normalize(pos.xyz);
        float noise = hash(pos.xyz + u_time);  // zależne od czasu

        float magnitude = 0.15; // jak bardzo wypychać
        vec3 distortion = offsetDir * (noise * magnitude);

        vec4 displaced = vec4(pos.xyz + distortion, 1.0);
        gl_Position = displaced;

        // możesz przekazać kolor jak w vertex shaderze
        FragColor = vec4(1.0); // lub cokolwiek masz z vertex shadera
        EmitVertex();
    }
    EndPrimitive();
}
