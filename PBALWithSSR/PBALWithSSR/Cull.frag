#version 420 core


layout(location = 0) out vec4 col;

void main(void)
{
	col = vec4(gl_FragCoord.z,gl_FragCoord.z,gl_FragCoord.z,gl_FragCoord.z);
//	col = vec4(gl_FragCoord.z/gl_FragCoord.w,gl_FragCoord.z/gl_FragCoord.w,gl_FragCoord.z/gl_FragCoord.w,gl_FragCoord.z/gl_FragCoord.w);
}
