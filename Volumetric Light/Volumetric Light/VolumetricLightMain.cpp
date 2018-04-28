
#include <fstream>
#include <iostream>
#include <sstream>
#include"LoadShaders.h"
#include"vmath.h"
#include<vector>
#define BUFFER_OFFSET(offset) ((GLvoid*) (NULL + offset))
#define WIDTH 800
#define HEIGHT 600
#define LIGHTRADIO_VERTEX_SHADER "lightradio.vert"
#define LIGHTRADIO_FRAAGMENT_SHADER "lightradio.frag"
#define VOLUMETRICLIGHT_VERTEX_SHADER "volumetriclight.vert"
#define VOLUMETRICLIGHT_FRAGMENT_SHADER "volumetriclight.frag"
#define RENDERFORGAMEOBJECT_VERTEX_SHADER "renderforgameobject.vert"
#define RENDERFORGAMEOBJECT_FRAGMENT_SHADER "renderforgameobject.frag"
struct GLobalParameter
{
	vmath::vec3 LightDir;
	vmath::vec3 LightPos;
	vmath::vec3 viewpoint;
	vmath::vec3 backgroundColor;
	GLuint VAO[1];
	GLuint VBO[1];
	GLuint NUM_SAMPLES;
	GLfloat Decay;
	GLfloat weight;
	GLfloat Density;
	GLfloat Exposure;
	GLfloat maxRadioDistance;
	GLuint texture[3];
	GLuint maxRandomCount = 50;
	GLuint maxRandomSize = 4;
	GLuint maxRandomHeight = 8;
	GLuint totalSize = 40;
	GLuint FBO[3];
	GLuint lightradio_shader;
	GLuint volumetriclight_shader;
	GLuint renderforgameobject_shader;

	GLuint renderforgameobject_type_loc;
	GLuint renderforgameobject_projmat_loc;
	GLuint renderforgameobject_lightdir_loc;

	GLuint lightradio_maxDis_loc;
	GLuint lightradio_lightpos_loc;
	GLuint lightradio_mask_loc;

	GLuint volumetriclight_maintex_loc;
	GLuint volumetriclight_lightradiotex_loc;
	GLuint volumetriclight_lightcoord_loc;
	GLuint volumetriclight_numsamples_loc;
	GLuint volumetriclight_decay_loc;
	GLuint volumetriclight_weight_loc;
	GLuint volumetriclight_density_loc;
	GLuint volumetriclight_exposure_loc;
};
using namespace std;
using namespace vmath;


void InitConig()
{

}
void InitTexture()
{


}
void InitFBO()
{


}
void Mouse(int button, int state, int x, int y)
{

}
void onMouseMove(int x, int y)
{

}

void InitShader()
{
	
}
void Init()
{
	InitShader();
	InitTexture();
	InitFBO();
}
void Render_Object(int type)
{
	glViewport(0, 0, WIDTH, HEIGHT);
	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void displayFBO(GLuint fbo)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	Render_Object(1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void display()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	displayFBO(0);

	Render_Object(3);
	glEnd();
	glutSwapBuffers();
}
void myReshape(int w, int h)
{
	
}
void Idle()
{
	display();
}
void processNormalKeys(unsigned char key, int x, int y)
{
	glutPostRedisplay();
}
void processSpecialKeys(int key, int x, int y) 
{
	glutPostRedisplay();
}
int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("first");
	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	if (glewInit())
	{
		cerr << "Unable to initialize GLEW ... exiting" << endl;
		exit(EXIT_FAILURE);
	}
	Init();
	glutMouseFunc(Mouse);
	glutMotionFunc(onMouseMove);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutDisplayFunc(display);
	glutReshapeFunc(myReshape);
	glutIdleFunc(Idle);
	glutMainLoop();
	return 0;
}