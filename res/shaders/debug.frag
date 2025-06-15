#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform samplerCube depthMap;
uniform int faceIndex;

vec3 getCubeMapDir(vec2 uv, int face)
{
    uv = uv * 2.0 - 1.0; // z [0,1] → [-1,1]

    vec3 dir;
    if (face == 0)      dir = vec3( 1.0, -uv.y, -uv.x); // +X
    else if (face == 1) dir = vec3(-1.0, -uv.y,  uv.x); // -X
    else if (face == 2) dir = vec3( uv.x,  1.0,  uv.y); // +Y
    else if (face == 3) dir = vec3( uv.x, -1.0, -uv.y); // -Y
    else if (face == 4) dir = vec3( uv.x, -uv.y,  1.0); // +Z
    else                dir = vec3(-uv.x, -uv.y, -1.0); // -Z

    return normalize(dir);
}

void main()
{

    vec3 dir = getCubeMapDir(TexCoords, 5);
    float depth = texture(depthMap, dir).r;
    FragColor = vec4(vec3(1.f - depth), 1.f);
}
