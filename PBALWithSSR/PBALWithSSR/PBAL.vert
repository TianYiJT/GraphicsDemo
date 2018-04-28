#version 420 core


uniform mat4 mvp;
uniform vec3 viewpoint;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;
layout(location = 2) in vec3 col;
layout(location = 3) in float roughness;

out vec3 Nor;
out float Roughness;
out vec3 View;
out vec3 position;
out vec3 color_;

void main(void)
{
	gl_Position = mvp * vec4(pos,1.0f);
	Nor = nor;
	Roughness = roughness;
	View = viewpoint - pos;
	position = pos;
	color_ = col;
}
