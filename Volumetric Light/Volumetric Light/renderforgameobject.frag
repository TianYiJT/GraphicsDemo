#version 420 core
uniform int type;
uniform mat4 proj_mat;
uniform vec3 lightdir;

in vec3 normal_out;
in vec3 color_out;

layout(location = 0) out vec4 col;

void main(void)
{
	if(type==0)
	{
		col = vec4(0.0f,0.0f,0.0f,1.0f);
	//	col = vec4(lightdir,1.0f);
	}
	else
	{
		vec3 N = normalize(normal_out);
		vec3 D = normalize(lightdir);
		float NdotD = clamp(dot(N,D),0,1);
		col = vec4(color_out,1.0f)*NdotD;
		//col = vec4(color_out,1.0f);
		//col = vec4(0.0f,0.0f,0.0f,1.0f);
	}
}
