#version 420 core
in vec3 vs_fs_normal;
layout(location=0) out vec4 col;

void main()
{
	col=vec4(0.3f,0.6f,0.0f,1.0f)+dot(vs_fs_normal,vec3(0.0f,0.0f,-1.0f))*vec4(0.0f,0.0f,0.8f,1.0f);
	col=normalize(col);
}
