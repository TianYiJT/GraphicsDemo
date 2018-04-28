#version 420 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

uniform vec2 lightpos;

uniform float maxDistance;

uniform sampler2D texture_mask;

out vec2 texcoord;

void main(void)
{
	gl_Position=position;
	texcoord = texCoord;
}
