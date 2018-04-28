/*
#include<iostream>
#include"stdio.h"
#include"LoadShaders.h"
#include"vmath.h"
#define BUFFER_OFFSET(offset) ((GLvoid*) (NULL + offset))

using namespace std;
using namespace vmath;
float aspect;
GLuint render_prog;
GLuint vao[1];
GLuint vbo[1];
GLuint ebo[1];

GLint render_model_matrix_loc;
GLint render_projection_matrix_loc;
GLint render_light_color;
GLint render_light_view;
void display() 
{
	static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
	static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
	static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);
	static const vmath::vec4 black = vmath::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	mat4 model_matrix;

	// Setup
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glClearBufferfv(GL_COLOR, 0, black);

	// Activate simple shading program
	glUseProgram(render_prog);

	// Set up the model and projection matrix
	vmath::mat4 projection_matrix(vmath::frustum(-20.0f, 20.0f, -aspect, aspect, 1.0f, 50.0f));
	glUniformMatrix4fv(render_projection_matrix_loc, 1, GL_FALSE, projection_matrix);

	// Set up for a glDrawElements call
	glBindVertexArray(vao[0]);
	// 将索引数组绑定到缓存上
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);

	// Draw Arrays...
	model_matrix = vmath::translate(-3.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);

	glUniform3f(render_light_view,0.0f,-1.0f,-1.0f);
	glUniform4f(render_light_color, 1.0f, 1.0f, 1.0f,1.0f);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// DrawElements
	// 都去顶点中第0,1,2号元素
	model_matrix = vmath::translate(-1.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);
	// DrawElementsBaseVertex
	// 读取的索引为1,2,3，所以第三个三角形颜色不一样
	model_matrix = vmath::translate(1.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glDrawElementsBaseVertex(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL, 1);

	// DrawArraysInstanced
	model_matrix = vmath::translate(3.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);
	glFlush();
}
void Init() 
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH, GL_NICEST);
	ShaderInfo shader_info[] =
	{
		{ GL_VERTEX_SHADER, "VertexDemo.vs" },
		{ GL_FRAGMENT_SHADER, "FragmentDemo.fs" },
		{ GL_NONE, NULL }
	};

	render_prog = LoadShaders(shader_info);

	glUseProgram(render_prog);

	// "model_matrix" is actually an array of 4 matrices
	render_model_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");
	render_projection_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");
	render_light_color = glGetUniformLocation(render_prog, "lightcolor");
	render_light_view = glGetUniformLocation(render_prog, "lightpos");
	// A single triangle
	static const GLfloat vertex_positions[] =
	{
		-1.0f, -1.0f,  0.0f, 
		1.0f, -1.0f,  0.0f, 
		-1.0f,  1.0f,  0.0f,
		-1.0f, -1.0f,  0.0f,
	};

	// Color for each vertex
	static const GLfloat vertex_normals[] =
	{
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 
		1.0f, 0.0f, 1.0f, 
		0.0f, 1.0f, 1.0f, 
	};
	static const GLfloat vertex_views[] =
	{
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
	};
	// Indices for the triangle strips
	static const GLushort vertex_indices[] =
	{
		0, 1,2
	};

	// Set up the element array buffer
	// 索引数组的意义在于减少重复的顶点存储
	glGenBuffers(1, ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);

	// Set up the vertex attributes
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	// 将颜色和位置信息都绑定到一起
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions) + sizeof(vertex_normals)+sizeof(vertex_views), NULL, GL_STATIC_DRAW);
	// 前半段设置为位置信息
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_positions), vertex_positions);
	// 后半段设置为颜色信息
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_positions), sizeof(vertex_normals), vertex_normals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_positions)+ sizeof(vertex_normals), sizeof(vertex_views), vertex_views);
	// 将位置信息传入
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	// 将颜色信息传入
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(vertex_positions));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(sizeof(vertex_positions) + sizeof(vertex_normals)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}
void reshape(int x, int y) 
{
	glViewport(0, 0, x, y);
	aspect = float(x) / float(y);
}
int main(int argc,char ** argv) 
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GL_RGBA);

	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(512, 512);
	glutCreateWindow("first");
	if (glewInit()) 
	{
		cerr << "Unable to initialize GLEW ... exiting" << endl; 
		exit(EXIT_FAILURE); 
	}
	Init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}*/