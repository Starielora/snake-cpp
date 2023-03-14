// #version 330 core
// layout (location = 0) in vec3 aPos;

// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;

// out vec4 col;

// void main()
// {
// 	gl_Position = projection * view * model * vec4(aPos, 1.0);
//     col = 2*vec4(aPos, 1.0);
// }

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 instanceColor;
layout (location = 2) in mat4 instanceModel;

uniform mat4 view;
uniform mat4 projection;

out vec4 outColor;

void main()
{
	gl_Position = projection * view * instanceModel * vec4(aPos, 1.0f);
	outColor = instanceColor;
}
