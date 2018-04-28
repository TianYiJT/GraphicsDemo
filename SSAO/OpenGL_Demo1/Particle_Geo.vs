#version 420 core
uniform mat4 model_matrix;
uniform mat4 projection_matrix;
layout(location=0) in vec4 position;
layout(location=1) in vec3 normal;

out vec4 world_space_position;
out vec3 vs_fs_normal;

void main()
{
	vec4 pos=(model_matrix*(position*vec4(1.0f,1.0f,1.0f,1.0f)));
	world_space_position=pos;
	vs_fs_normal=normalize(model_matrix*vec4(normal,0.0f).xyz);
	gl_Position=projection_matrix*world_space_position;
} 
 