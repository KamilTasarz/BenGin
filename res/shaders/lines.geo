#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 64) out;

uniform mat4 view;
uniform mat4 projection;
float radius = 0.05;

const int SIDES = 8;

void main() {
    vec3 p0 = vec3(gl_in[0].gl_Position);
    vec3 p1 = vec3(gl_in[1].gl_Position);

    vec3 dir = normalize(p1 - p0);

    vec3 up = abs(dir.y) < 0.99 ? vec3(0, 1, 0) : vec3(1, 0, 0);
    vec3 right = normalize(cross(dir, up));
    vec3 forward = normalize(cross(right, dir));

    vec3 circle0[SIDES];
    vec3 circle1[SIDES];

    for (int i = 0; i < SIDES; ++i) {
        float angle = 6.2831853 * i / float(SIDES); // 2*pi
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;
        vec3 offset = right * x + forward * y;
        circle0[i] = p0 + offset;
        circle1[i] = p1 + offset;
    }

    for (int i = 0; i <= SIDES; ++i) {
        int j = i % SIDES;

        gl_Position = projection * view * vec4(circle0[j], 1.0);
        EmitVertex();

        gl_Position = projection * view * vec4(circle1[j], 1.0);
        EmitVertex();
    }

    EndPrimitive();
}
