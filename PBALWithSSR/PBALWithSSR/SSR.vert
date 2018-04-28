#version 420

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
out vec2 uv;

void main(void)
{
	gl_Position = position;
	uv = texCoord;
//	uv = texCoord;
	//screenPos=position.xy;
}
