
#include<iostream>
#include"stdio.h"
#include"LoadShaders.h"
#include"vmath.h"

#define BUFFER_OFFSET(offset) ((GLvoid*) (NULL + offset))
GLuint render_prog;
GLint render_model_matrix_loc;
GLint render_projection_matrix_loc;
using namespace std;
static const GLfloat vertex_positions[] = { -1.0f, -1.0f, 0.0f, 1.0f,
											1.0f, -1.0f, 0.0f, 1.0f,
											-1.0f, 1.0f, 0.0f, 1.0f, 
											-1.0f, -1.0f, 0.0f, 1.0f, };


static const GLfloat vertex_colors[] = {1.0f, 1.0f, 1.0f, 1.0f,
										1.0f, 1.0f, 0.0f, 1.0f, 
										1.0f, 0.0f, 1.0f, 1.0f,
										0.0f, 1.0f, 1.0f, 1.0f };


static const GLushort vertex_indices[] = { 0, 1, 2 };
GLuint *ebo = new GLuint[1];
GLuint *vao = new GLuint[1];
GLuint *vbo = new GLuint[1];
void Init() 
{

	glGenBuffers(1, ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions) + sizeof(vertex_colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(vertex_positions),vertex_positions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_positions), sizeof(vertex_colors), vertex_colors);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(vertex_positions));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	ShaderInfo shaders[] = { { GL_VERTEX_SHADER, "VertexDemo.vs" },
	{ GL_FRAGMENT_SHADER, "FragmentDemo.fs" },
	{ GL_NONE, NULL } };
	render_prog = LoadShaders(shaders);
	glUseProgram(render_prog);
	render_model_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");
	render_projection_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");
}
void display() 
{
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	vmath::mat4 model_matrix;
	model_matrix = vmath::translate(-3.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 4, GL_FALSE, model_matrix); 
	glDrawArrays(GL_TRIANGLES, 0, 3);

	model_matrix = vmath::translate(-1.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 4, GL_FALSE, model_matrix); 
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);

	model_matrix = vmath::translate(1.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 4, GL_FALSE, model_matrix); 
	glDrawElementsBaseVertex(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL, 1);

	model_matrix = vmath::translate(3.0f, 0.0f, -5.0f);
	glUniformMatrix4fv(render_model_matrix_loc, 4, GL_FALSE, model_matrix); 
	glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);
	glFlush();
}
 
int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GL_RGBA);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(argv[0]);
	if (glewInit())
	{
		cerr << "Unable to initialize GLEW ... exiting" << endl;
		exit(EXIT_FAILURE);
	}
	Init();
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}