/*
#include<iostream>
#include"stdio.h"
#include"LoadShaders.h"
#include"vmath.h"
#define BUFFER_OFFSET(offset) ((GLvoid*) (NULL + offset))
#define ALL_FACE 12
#define FRONT_FIR 0
#define FRONT_SEC 3
#define BACK_FIR 6
#define BACK_SEC 9
#define RIGHT_FIR 12
#define RIGHT_SEC 15
#define LEFT_FIR 18
#define LEFT_SEC 21
#define UP_FIR 24
#define UP_SEC 27
#define DOWN_FIR 30
#define DOWN_SEC 33
using namespace std;
float aspect_this;
namespace transform
{

	float aspect = 800.0 / 600.0;
	GLuint update_prog;
	GLuint vao[2];
	GLuint vbo[2];
	GLuint xfb;
	GLuint geometry_vbo;
	GLuint render_prog;
	GLuint render_vao;
	GLuint render_vbo;
	GLint render_model_matrix_loc;
	GLint render_projection_matrix_loc;
	GLint model_matrix_loc;
	GLint projection_matrix_loc;
	GLint triangle_count_loc;
	GLint time_step_loc;
	GLint samplerbuffer_loc;
	const int point_count = 5000;
	static unsigned int seed = 0x13371337;
	vmath::vec4 render_data_position[3 * ALL_FACE];
	vmath::vec3 render_data_normal[3 * ALL_FACE];
};

using namespace transform;

static inline float random_float()
{
	float res;
	unsigned int tmp;
	
	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	*((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

	return (res - 1.0f);
}

static vmath::vec3 random_vector(float minmag = 0.0f, float maxmag = 1.0f)
{
	vmath::vec3 randomvec(random_float() * 2.0f - 1.0f, random_float() * 2.0f - 1.0f, random_float() * 2.0f - 1.0f);
	randomvec = normalize(randomvec);
	randomvec *= (random_float() * (maxmag - minmag) + minmag);

	return randomvec;
}

static inline int min(int a, int b)
{
	return a < b ? a : b;
}
void InitCube() 
{
	render_data_position[FRONT_FIR] = vmath::vec4{-0.5f,0.5f,0.5f,1.0f};
	render_data_normal[FRONT_FIR] = vmath::vec3{0.0f,0.0f,1.0f}; 
	render_data_position[FRONT_FIR+1] = vmath::vec4{ 0.5f,0.5f,0.5f,1.0f };
	render_data_normal[FRONT_FIR+1] = vmath::vec3{ 0.0f,0.0f,1.0f };
	render_data_position[FRONT_FIR+2] = vmath::vec4{ -0.5f,-0.5f,0.5f,1.0f };
	render_data_normal[FRONT_FIR+2] = vmath::vec3{ 0.0f,0.0f,1.0f };

	render_data_position[FRONT_SEC] = vmath::vec4{ 0.5f,-0.5f,0.5f,1.0f };
	render_data_normal[FRONT_SEC] = vmath::vec3{ 0.0f,0.0f,1.0f };
	render_data_position[FRONT_SEC + 1] = vmath::vec4{ 0.5f,0.5f,0.5f,1.0f };
	render_data_normal[FRONT_SEC + 1] = vmath::vec3{ 0.0f,0.0f,1.0f };
	render_data_position[FRONT_SEC + 2] = vmath::vec4{ -0.5f,-0.5f,0.5f,1.0f };
	render_data_normal[FRONT_SEC + 2] = vmath::vec3{ 0.0f,0.0f,1.0f };

	render_data_position[BACK_FIR] = vmath::vec4{ -0.5f,0.5f,-0.5f,1.0f };
	render_data_normal[BACK_FIR] = vmath::vec3{ 0.0f,0.0f,-1.0f };
	render_data_position[BACK_FIR + 1] = vmath::vec4{ 0.5f,0.5f,-0.5f,1.0f };
	render_data_normal[BACK_FIR + 1] = vmath::vec3{ 0.0f,0.0f,-1.0f };
	render_data_position[BACK_FIR + 2] = vmath::vec4{ -0.5f,-0.5f,-0.5f,1.0f };
	render_data_normal[BACK_FIR + 2] = vmath::vec3{ 0.0f,0.0f,-1.0f };

	render_data_position[BACK_SEC] = vmath::vec4{ 0.5f,-0.5f,-0.5f,1.0f };
	render_data_normal[BACK_SEC] = vmath::vec3{ 0.0f,0.0f,-1.0f };
	render_data_position[BACK_SEC + 1] = vmath::vec4{ 0.5f,0.5f,-0.5f,1.0f };
	render_data_normal[BACK_SEC + 1] = vmath::vec3{ 0.0f,0.0f,-1.0f };
	render_data_position[BACK_SEC + 2] = vmath::vec4{ -0.5f,-0.5f,-0.5f,1.0f };
	render_data_normal[BACK_SEC + 2] = vmath::vec3{ 0.0f,0.0f,-1.0f };

	render_data_position[UP_FIR] = vmath::vec4{ -0.5f,0.5f,-0.5f,1.0f };
	render_data_normal[UP_FIR] = vmath::vec3{ 0.0f,1.0f,0.0f };
	render_data_position[UP_FIR + 1] = vmath::vec4{ -0.5f,0.5f,0.5f,1.0f };
	render_data_normal[UP_FIR + 1] = vmath::vec3{ 0.0f,1.0f,0.0f };
	render_data_position[UP_FIR + 2] = vmath::vec4{ 0.5f,0.5f,-0.5f,1.0f };
	render_data_normal[UP_FIR + 2] = vmath::vec3{ 0.0f,1.0f,0.0f };

	render_data_position[UP_SEC] = vmath::vec4{ 0.5f,0.5f,0.5f,1.0f };
	render_data_normal[UP_SEC] = vmath::vec3{ 0.0f,1.0f,0.0f };
	render_data_position[UP_SEC + 1] = vmath::vec4{ 0.5f,0.5f,-0.5f,1.0f };
	render_data_normal[UP_SEC + 1] = vmath::vec3{ 0.0f,1.0f,0.0f };
	render_data_position[UP_SEC + 2] = vmath::vec4{ -0.5f,0.5f,0.5f,1.0f };
	render_data_normal[UP_SEC + 2] = vmath::vec3{ 0.0f,1.0f,0.0f };

	render_data_position[DOWN_FIR] = vmath::vec4{ -0.5f,-0.5f,-0.5f,1.0f };
	render_data_normal[DOWN_FIR] = vmath::vec3{ 0.0f,-1.0f,0.0f };
	render_data_position[DOWN_FIR + 1] = vmath::vec4{ -0.5f,-0.5f,0.5f,1.0f };
	render_data_normal[DOWN_FIR + 1] = vmath::vec3{ 0.0f,-1.0f,0.0f };
	render_data_position[DOWN_FIR + 2] = vmath::vec4{ 0.5f,-0.5f,-0.5f,1.0f };
	render_data_normal[DOWN_FIR + 2] = vmath::vec3{ 0.0f,-1.0f,0.0f };

	render_data_position[DOWN_SEC] = vmath::vec4{ 0.5f,-0.5f,0.5f,1.0f };
	render_data_normal[DOWN_SEC] = vmath::vec3{ 0.0f,-1.0f,0.0f };
	render_data_position[DOWN_SEC + 1] = vmath::vec4{ 0.5f,-0.5f,-0.5f,1.0f };
	render_data_normal[DOWN_SEC + 1] = vmath::vec3{ 0.0f,-1.0f,0.0f };
	render_data_position[DOWN_SEC + 2] = vmath::vec4{ -0.5f,-0.5f,0.5f,1.0f };
	render_data_normal[DOWN_SEC + 2] = vmath::vec3{ 0.0f,-1.0f,0.0f };

	render_data_position[RIGHT_FIR] = vmath::vec4{ 0.5f,0.5f,0.5f,1.0f };
	render_data_normal[RIGHT_FIR] = vmath::vec3{ 1.0f,0.0f,0.0f };
	render_data_position[RIGHT_FIR + 1] = vmath::vec4{ 0.5f,0.5f,-0.5f,1.0f };
	render_data_normal[RIGHT_FIR + 1] = vmath::vec3{ 1.0f,0.0f,0.0f };
	render_data_position[RIGHT_FIR + 2] = vmath::vec4{ 0.5f,-0.5f,0.5f,1.0f };
	render_data_normal[RIGHT_FIR + 2] = vmath::vec3{ 1.0f,0.0f,0.0f };

	render_data_position[RIGHT_SEC] = vmath::vec4{ 0.5f,-0.5f,-0.5f,1.0f };
	render_data_normal[RIGHT_SEC] = vmath::vec3{ 1.0f,0.0f,0.0f };
	render_data_position[RIGHT_SEC + 1] = vmath::vec4{ 0.5f,-0.5f,0.5f,1.0f };
	render_data_normal[RIGHT_SEC + 1] = vmath::vec3{ 1.0f,0.0f,0.0f };
	render_data_position[RIGHT_SEC + 2] = vmath::vec4{ 0.5f,0.5f,-0.5f,1.0f };
	render_data_normal[RIGHT_SEC + 2] = vmath::vec3{ 1.0f,0.0f,0.0f };

	render_data_position[LEFT_FIR] = vmath::vec4{ -0.5f,0.5f,0.5f,1.0f };
	render_data_normal[LEFT_FIR] = vmath::vec3{ -1.0f,0.0f,0.0f };
	render_data_position[LEFT_FIR + 1] = vmath::vec4{ -0.5f,0.5f,-0.5f,1.0f };
	render_data_normal[LEFT_FIR + 1] = vmath::vec3{ -1.0f,0.0f,0.0f };
	render_data_position[LEFT_FIR + 2] = vmath::vec4{ -0.5f,-0.5f,0.5f,1.0f };
	render_data_normal[LEFT_FIR + 2] = vmath::vec3{ -1.0f,0.0f,0.0f };

	render_data_position[LEFT_SEC] = vmath::vec4{ -0.5f,-0.5f,-0.5f,1.0f };
	render_data_normal[LEFT_SEC] = vmath::vec3{ -1.0f,0.0f,0.0f };
	render_data_position[LEFT_SEC + 1] = vmath::vec4{ -0.5f,-0.5f,0.5f,1.0f };
	render_data_normal[LEFT_SEC + 1] = vmath::vec3{ -1.0f,0.0f,0.0f };
	render_data_position[LEFT_SEC + 2] = vmath::vec4{ -0.5f,0.5f,-0.5f,1.0f };
	render_data_normal[LEFT_SEC + 2] = vmath::vec3{ -1.0f,0.0f,0.0f };
}
void InitParticle() 
{
	glGenVertexArrays(2, vao);
	glGenBuffers(2, vbo);

	for (int i = 0; i < 2; i++)
	{
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, vbo[i]);
		glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, point_count * (2*sizeof(vmath::vec4) + sizeof(vmath::vec3)), NULL, GL_DYNAMIC_COPY);
		GLenum errCode = glGetError();

		if (0 == i)
		{
			struct buffer_t
			{
				vmath::vec4 position;
				vmath::vec3 velocity;
				vmath::vec4 col;
			};
			buffer_t* buffer = (buffer_t*)glMapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_WRITE_ONLY);
			for (int j = 0; j < point_count; j++)
			{
				buffer[j].velocity = random_vector();
				buffer[j].position = vmath::vec4(vmath::vec3(-0.5f, 12.0f, 0.0f), 1.0f);
				buffer[j].velocity = vmath::vec3(buffer[j].velocity[0]*0.03f, buffer[j].velocity[1] * 0.03f, buffer[j].velocity[2] * 0.03f);
				buffer[j].col = vmath::vec4(random_vector(),1.0f);
			}

			glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
		}
		glBindVertexArray(vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 2*sizeof(vmath::vec4) + sizeof(vmath::vec3), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vmath::vec4) + sizeof(vmath::vec3), (GLvoid*)sizeof(vmath::vec4));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, 2*sizeof(vmath::vec4) + sizeof(vmath::vec3), (GLvoid*)(sizeof(vmath::vec4)+sizeof(vmath::vec3)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
	}
}
void InitModel() 
{
	glGenVertexArrays(1, &render_vao);
	glBindVertexArray(render_vao);
	glGenBuffers(1, &render_vbo);
	glBindBuffer(GL_TEXTURE_BUFFER, render_vbo);
	glBufferData(GL_TEXTURE_BUFFER,sizeof(render_data_position)+sizeof(render_data_normal),NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_TEXTURE_BUFFER, 0, sizeof(render_data_position), render_data_position);
	glBufferSubData(GL_TEXTURE_BUFFER,sizeof(render_data_position),sizeof(render_data_normal),render_data_normal);
	glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,sizeof(vmath::vec4),NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vmath::vec3), (GLvoid*)(sizeof(render_data_position)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}
void InitShaders() 
{
	ShaderInfo shader_info_update[] =
	{
		{ GL_VERTEX_SHADER, "Particle_Run.vs" },
		{ GL_FRAGMENT_SHADER, "Particle_Color.fs" },
		{ GL_NONE, NULL }
	};
	update_prog = LoadShaders(shader_info_update);
	static const char* varyings[] =
	{
		"position_out", "velocity_out","particle_col"
	};
	glTransformFeedbackVaryings(update_prog, 3, varyings, GL_INTERLEAVED_ATTRIBS);
	glLinkProgram(update_prog);
	glUseProgram(update_prog);
	model_matrix_loc = glGetUniformLocation(update_prog, "model_matrix");
	projection_matrix_loc = glGetUniformLocation(update_prog, "projection_matrix");
	triangle_count_loc = glGetUniformLocation(update_prog, "triangle_count");
	time_step_loc = glGetUniformLocation(update_prog, "time_step");
	samplerbuffer_loc = glGetUniformLocation(update_prog,"geometry_tbo");
	ShaderInfo shader_info_render[] = 
	{
		{ GL_VERTEX_SHADER, "Particle_Geo.vs" },
		{ GL_FRAGMENT_SHADER, "Particle_Geo.fs" },
		{ GL_NONE, NULL }
	};
	render_prog = LoadShaders(shader_info_render);
	static const char* varyings2[] =
	{
		"world_space_position"
	};
	glTransformFeedbackVaryings(render_prog, 1, varyings2, GL_INTERLEAVED_ATTRIBS);
	glLinkProgram(render_prog);
	glUseProgram(render_prog);
	render_model_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");
	render_projection_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");
}
void init() 
{
	InitCube();
	InitShaders();
	InitParticle();
	InitModel();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glGenBuffers(1, &geometry_vbo);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, geometry_vbo);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(render_data_position), NULL, GL_DYNAMIC_COPY);
}
void display() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	static int frame_count = 0;
	static float q = 0.0f;
	float t = float(GetTickCount() & 0x3FFFF) / float(0x3FFFF);
	vmath::mat4 projection_matrix(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 5000.0f) * vmath::translate(0.0f, 0.0f, -100.0f));
	vmath::mat4 model_matrix(vmath::scale(0.3f) *
		vmath::rotate(t * 360.0f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate(t * 360.0f * 3.0f, 0.0f, 0.0f, 1.0f));
	glUseProgram(render_prog);
	glUniformMatrix4fv(render_model_matrix_loc, 1, GL_FALSE, model_matrix);
	glUniformMatrix4fv(render_projection_matrix_loc, 1, GL_FALSE, projection_matrix);

	glBindVertexArray(render_vao);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, geometry_vbo);
	glBeginTransformFeedback(GL_TRIANGLES);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glEndTransformFeedback();
	
	glUseProgram(update_prog);
	model_matrix = vmath::mat4::identity();
	glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, model_matrix);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, projection_matrix);
	glUniform1i(triangle_count_loc, ALL_FACE);
	//cout << glGetError() << endl;
	if (t > q)
	{
		glUniform1f(time_step_loc, 0.2f);
		static TCHAR debugStr[4096] = L"";
		swprintf_s(debugStr, L" FrameCount = %.3f\n", t);
		OutputDebugString(debugStr);
	}
	q = t;
	if ((frame_count & 1) != 0)
	{
		glBindVertexArray(vao[1]);	
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[0]);
	}
	else
	{
		glBindVertexArray(vao[0]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[1]);
	}
	glBeginTransformFeedback(GL_POINTS);
	glPointSize(1.8f);
	glDrawArrays(GL_POINTS, 0, point_count);
	glEndTransformFeedback();
	glBindVertexArray(0);
	frame_count++;
	glFlush();
}
void reshape(int x,int y) 
{
	glViewport(0, 0, x, y);
	aspect_this = float(x) / float(y);
}
void Update() 
{
	static int x = 0;
	if (x%30==0)
	{
		display();
	}
	else
	{
		x++;
	}
}
int main(int argc, char ** argv) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GL_RGBA);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(argv[0]);
	//gluPerspective(60,120,1.0f,1000.0f);
	if (glewInit())
	{
		cerr << "Unable to initialize GLEW ... exiting" << endl;
		exit(EXIT_FAILURE);
	}
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(Update);
	glutMainLoop();
	return 0;
}*/
