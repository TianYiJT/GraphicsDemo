#version 420 core
uniform int type;
uniform mat4 proj_mat;
uniform vec3 lightdir;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;

out vec3 normal_out;
out vec3 color_out;

void main(void)
{
	gl_Position = proj_mat*vec4(position,1.0f);
	normal_out = normal;
	color_out = color;
}
