#version 420 core
uniform sampler2D texture_lightradio;
uniform sampler2D texture_main;
uniform float NUM_SAMPLES;
uniform float Decay;
uniform float weight;
uniform float Density;
uniform float Exposure;
uniform vec2 lightcoord;

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 texcoord;
void main()
{
	gl_Position  = position;
	texcoord = texCoord;
}
