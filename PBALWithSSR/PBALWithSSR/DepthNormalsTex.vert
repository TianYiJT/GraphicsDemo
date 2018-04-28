#version 420 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;

uniform mat4 mvp;
uniform mat4 normalMatrix;
out vec3 N;

void main(void)
{
	gl_Position = mvp*vec4(pos,1.0f);
//	gl_Position.z = 0.0;
	N = mat3(normalMatrix)*nor;
}
