#version 420 core

layout(location = 0) out vec4 col;

in vec3 N;


void main(void)
{
	
	
	col = vec4(N.xyz,gl_FragCoord.z);

//	col = vec4(gl_FragCoord.z,gl_FragCoord.z,gl_FragCoord.z,1.0);

}
