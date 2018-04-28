#version 420 core

layout(location = 0) out vec4 col;

in vec3 pos_;

void main(void)
{
	col = vec4(pos_.xyz,1.0f);
	//col = vec4(1.0,0.0,0.0,1.0);
//	col = vec4(pos_,1.0);
}
