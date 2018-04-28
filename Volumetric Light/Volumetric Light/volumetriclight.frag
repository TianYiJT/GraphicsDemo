#version 420 core
uniform sampler2D texture_lightradio;
uniform sampler2D texture_main;
uniform float NUM_SAMPLES;
uniform float Decay;
uniform float weight;
uniform float Density;
uniform float Exposure;
uniform vec2 lightcoord;
in vec2 texcoord;
layout(location = 0) out vec4 col;
vec2 tran(vec2 v)
{
	vec2 res = vec2(v.x/800.0f,v.y/600.0f);
	return res;
}
void main()
{
	vec2 uv = texcoord;
	vec2 deltaTexcoord = texcoord-tran(lightcoord);
	deltaTexcoord*= 1.0f/NUM_SAMPLES*Density;
	vec3  color = texture2D(texture_lightradio,uv).xyz;
	float illumationDecay = 1.0f;
	for(int i=0;i<NUM_SAMPLES;i++)
	{
		uv -= deltaTexcoord;
		vec3 samplecolor = texture2D(texture_lightradio,uv).xyz;
		samplecolor *=illumationDecay*weight;
		color+=samplecolor;
		illumationDecay*=Decay;
	}
	color*=Exposure;
	vec3 maincolor = texture2D(texture_main,texcoord).xyz;
//	maincolor /=256.0f;
	col = vec4((color+maincolor),1.0f);
	//col = vec4(maincolor,1.0f);
}
