#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 Position[];
in vec2 TextureCoords[];
in vec4 Color[];

out vec3 Position;
out vec2 TextureCoords;
out vec3 Normal;
out vec4 Color;

uniform mat4 view;
uniform mat4 projection;
uniform float size;

// promień "kolca"
float spike_length = 0.2;

void main()
{
    vec3 right = vec3(view[0][0], view[1][0], view[2][0]) * size;
    vec3 up    = vec3(view[0][1], view[1][1], view[2][1]) * size;

    vec3 pos = Position[0];

    vec3 corners[4] = vec3[](
        pos - right + up,
        pos + right + up,
        pos - right - up,
        pos + right - up
    );

    vec2 uvs[4] = vec2[](
        vec2(0, 1),
        vec2(1, 1),
        vec2(0, 0),
        vec2(1, 0)
    );

    for (int i = 0; i < 4; ++i) {
        Position = corners[i];
        TextureCoords = uvs[i];
        Normal = vec3(0.0, 0.0, 1.0);
        Color = Color[0];
        gl_Position = projection * view * vec4(corners[i], 1.0);
        EmitVertex();
    }
    EndPrimitive();
}
