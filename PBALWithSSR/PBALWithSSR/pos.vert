#version 420 core

layout(location = 0) in vec3 pos;

uniform mat4 mvp;
uniform mat4 mvMatrix;

out vec3 pos_;
void main(void)
{
	gl_Position = mvp * vec4(pos,1.0);
	pos_  = (mvMatrix * vec4(pos,1.0)).xyz;
}
