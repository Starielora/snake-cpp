#version 460 core

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;

layout(location = 0) in vec3 aPos;
layout(location = 1) out vec3 color;

void main()
{
    vec3 pos = aPos;
    pos.y /= 8.;
    gl_Position = (projection * view * vec4(pos + vec3(cameraPos.x, 0, cameraPos.z), 1.0));
}
