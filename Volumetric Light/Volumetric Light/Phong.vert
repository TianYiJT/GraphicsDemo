#version 420 core
uniform mat4 model_matrix; 
uniform mat4 projection_matrix; 
uniform vec3 lightpos;
uniform vec4 lightcolor;
uniform sampler2D Depth;
uniform sampler2D Normal_Texture;
uniform int shader_type;
uniform vec3 cameraposition;
layout (location=0) in vec3 position;
layout(location=1)  in vec3 nor;
layout(location=2)  in vec3 v;
out vec3 N;
out vec3 view;
void main()
{
	gl_Position=projection_matrix*model_matrix*vec4(position,1.0f);
	N=nor;
	view=v;
}
