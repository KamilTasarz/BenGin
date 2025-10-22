#version 330 core
in vec2 TexCords;

out vec4 FragColor;

uniform sampler2D texture1;
uniform int hasTex;

void main()
{
    if (hasTex == 0)
        FragColor = vec4(1., 1., 1., 1.0);
    else {
        FragColor = texture(texture1, TexCords);
    }
}