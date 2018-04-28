#version 420
in vec4 particle_col;
layout(location=0) out vec4 col;

void main()
{
	col=particle_col;
}