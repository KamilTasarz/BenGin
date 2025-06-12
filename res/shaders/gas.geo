#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 view;
uniform mat4 projection;

// promień "kolca"
uniform float spike_length = 0.2;

void main()
{
    vec3 center = vec3(0.0); // środek sześcianu

    for (int i = 0; i < 1; i++) {
        vec3 base = gl_in[i].gl_Position.xyz;
        vec3 dir = normalize(base - center); // wektor normalny wychodzący z centrum

        // znajdź 2 wektory ortogonalne do dir, np. za pomocą Gram-Schmidta
        vec3 tangent = normalize(cross(dir, vec3(0.0, 1.0, 0.0)));
        if (length(tangent) < 0.001)
            tangent = normalize(cross(dir, vec3(1.0, 0.0, 0.0)));
        vec3 bitangent = normalize(cross(dir, tangent));

        float w = 0.05; // szerokość podstawy kolca

        // 3 punkty trójkąta
        vec3 tip = base + dir * spike_length;
        vec3 p1 = base + tangent * w;
        vec3 p2 = base + bitangent * w;

        gl_Position = projection * view * vec4(p1, 1.0);
        EmitVertex();
        gl_Position = projection * view * vec4(p2, 1.0);
        EmitVertex();
        gl_Position = projection * view * vec4(tip, 1.0);
        EmitVertex();
    }

    
    EndPrimitive();
}
