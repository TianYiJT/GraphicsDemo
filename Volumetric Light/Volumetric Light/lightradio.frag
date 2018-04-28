#version 420 core


layout(location=0) out vec4 col;

uniform vec2 lightpos;

uniform float maxDistance;

uniform sampler2D texture_mask;

in vec2 texcoord;

#define E 2.71828

float computeLightDensity(vec2 my,vec2 light)
{
	float d = distance(vec2(my.x*800.0f,my.y*600.0f),vec2(light.x*800.0f,light.y*600.0f));
	float density = pow(E,-d*d/(maxDistance*maxDistance*2));
	return density; 
}

vec2 tran(vec2 v)
{
	return vec2(v.x/800.0f,v.y/600.0f);
}

void main()
{
	vec2 light = tran(lightpos);
	vec2 my = (texcoord);
	float density = computeLightDensity(my,light);
	vec4 col_mask = texture2D(texture_mask,my);
	col = col_mask * density;
}
