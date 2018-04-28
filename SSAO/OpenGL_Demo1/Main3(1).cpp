
#include <fstream>
#include <iostream>
#include <sstream>
#include"LoadShaders.h"
#include"vmath.h"
#include<vector>
#include<fbxsdk.h>
#define BUFFER_OFFSET(offset) ((GLvoid*) (NULL + offset))
#define WIDTH 800
#define HEIGHT 600

using namespace std;
using namespace vmath;
const int BUFSIZE = 50 << 20;
char Buf[BUFSIZE + 1], *buf = Buf;
float max_x = -1, max_y = -1, max_z = -1;
float min_x = 999999, min_y = 999999, min_z = 9999999;
int MAX_LINE = 1200;
int MAX_POINT = 3000;
vmath::vec3 ** vertex_list;
vmath::vec3 ** normal_list;
vmath::vec3 ** view_list;
vmath::vec3  normal_map[WIDTH*HEIGHT];
GLuint Phong_Render;
GLuint LineAO_Render;
GLuint LineAO_Phong_Render;
GLuint update_prog;
int all_point=0;
vmath::vec3 cameraposition;
vmath::vec3 sceneposition;
vmath::vec3 up;
vmath::vec4 lightcol = {1.0f,1.0f,1.0f,1.0f};
vmath::vec3 lightpos = { 0.4f,0.7f,-0.1f };
GLfloat	cameraPerspective[4] = { 45.0, 1.0, 1.0, 1000.0 };
float angle_x;
float angle_y;
float aspect = WIDTH / HEIGHT;
GLuint light_pos;
GLuint light_color;
GLuint proj_loc;
GLuint model_matrix;
GLuint vertex_pos;
GLuint vertex_normal;
GLuint vertex_view;
GLuint VAO[1];
GLuint VBO[1];
GLuint Texture_Depth[1];
GLuint Texture_Color[1];
GLuint texture_depth_loc;
GLuint shader_type_loc;
GLuint DEPTHFBO[1];
GLuint COLORFBO[1];
GLuint Texture_COLOR_FXAA[1];
GLuint Texture_DEPTH_FXAA[1];
GLuint Normal_Texture_Loc;
GLuint cam_loc;
GLuint real_render_frame;
GLuint FXAA_Render;
GLuint Frame_Render_Loc;
GLuint rfc_loc;
GLuint linewidth_loc;
int old_x;
int old_y;
float fRotateAngle = 0.0f;
float m_aspectRatio = (float)WIDTH / (float)HEIGHT;
bool readStreamlinesFile(char *&d) 
{
	for (; *buf == ' ' || *buf == '\n'; buf++);
	if (*buf == '\0' || *buf == EOF) return false;
	d = buf;
	for (; *buf != ' '&&*buf != '\n'&&*buf != '\0'&&*buf != EOF; buf++);
	*buf = '\0'; buf++;
	return true;
}
vmath::vec3 getNormal(vmath::vec3 my,vmath::vec3 last,vmath::vec3 _this) 
{
	vmath::vec3 tangent = last - my;
	vmath::vec3 view = cameraposition - _this;
	vmath::vec3 vCrosst = vmath::cross(tangent,view);
	return vmath::normalize(vmath::cross(vmath::normalize(vCrosst),tangent));
}
void InitConig()
{
	cameraposition = vmath::vec3{ max_x*2 , max_y*2,max_z+min_z };
	sceneposition = vmath::vec3{ (max_x + min_x) / 2 ,(max_y + min_y) / 2 ,(max_z + min_z) / 2 };
	up = vmath::vec3{ 0.0f,0.0f,1.0f };
}
void InitTexture()
{
	glGenTextures(1, Texture_Depth);
	glBindTexture(GL_TEXTURE_2D, Texture_Depth[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1,Texture_Color);
	glBindTexture(GL_TEXTURE_2D,Texture_Color[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//Texture_Color[0] = generateMultiSampleTexture(4);
	/*
	glGenTextures(1, Texture_DEPTH_FXAA);
	glBindTexture(GL_TEXTURE_2D, Texture_DEPTH_FXAA[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, Texture_COLOR_FXAA);
	glBindTexture(GL_TEXTURE_2D, Texture_COLOR_FXAA[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0,
		GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	*/
}
void InitFBO()
{
	glGenFramebuffers(1, DEPTHFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, DEPTHFBO[0]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,Texture_Color[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Texture_Depth[0], 0);
	
	glGenFramebuffers(1, COLORFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, COLORFBO[0]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture_COLOR_FXAA[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Texture_DEPTH_FXAA[0], 0);
	
}
void Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
		old_x = x, old_y = y;
	//cout << "down:" << old_x << "," << old_y << endl;
}
void onMouseMove(int x, int y) 
{
	//cout << "move:" << x << "," << y << endl;
	angle_y += (x - old_x);
	angle_x += (y - old_y);
	//cout << angle_x << "," << angle_y << endl;
	old_x = x, old_y = y;
	glutPostRedisplay();
}
void makeData()
{
	float x, y, z;
	char *s;
	int line_num = 0;
	int point_num = 0;
	bool is_new_line = true;
	FILE *stream;
	stream = fopen("tornado.obj", "rb");
	cout << "readData" << endl;
	fread(Buf, 1, BUFSIZE, stream);
	while (true&&line_num<300)
	{
		if (!readStreamlinesFile(s)) break;
		if (is_new_line) 
		{
			is_new_line = false;
			vertex_list[line_num][0] = vmath::vec3{(float)point_num, 0.0f, 0.0f};
			point_num = 0;
		}
		
		if (*(s + 1) == '\0') {
			if (*s == 'v')
			{
				readStreamlinesFile(s); x = atof(s);
				readStreamlinesFile(s); y = atof(s);
				readStreamlinesFile(s); z = atof(s);
				if (x > max_x)
					max_x = x;
				if (x < min_x)
					min_x = x;
				if (y > max_y)
					max_y = y;
				if (y < min_y)
					min_y = y;
				if (z > max_z)
					max_z = z;
				if (z < min_z)
					min_z = z;
				vmath::vec3 point = { x,y,z };
				vertex_list[line_num + 1][point_num + 1] = point;
				point_num++;
			}
			else if (*s == 'g' || *s == '#') 
			{
				is_new_line = true;
				line_num++;
			}
		}
	}
	vertex_list[0][0] = { (float)line_num,0.0f,0.0f };
	normal_list[0][0] = { (float)line_num,0.0f,0.0f };
	view_list[0][0] = { (float)line_num,0.0f,0.0f };
	InitConig();
	cout << "line number:" << line_num << endl;
}
void InitShader()
{
	ShaderInfo shader_info_Phong[] =
	{
		{ GL_VERTEX_SHADER, "Phong.vert" },
		{ GL_FRAGMENT_SHADER, "Phong.frag" },
		{ GL_NONE, NULL }
	};
	Phong_Render = LoadShaders(shader_info_Phong);
	glUseProgram(Phong_Render);
	light_pos = glGetUniformLocation(Phong_Render, "lightpos");
	light_color = glGetUniformLocation(Phong_Render, "lightcolor");
	proj_loc = glGetUniformLocation(Phong_Render, "projection_matrix");
	model_matrix = glGetUniformLocation(Phong_Render, "model_matrix");
	texture_depth_loc = glGetUniformLocation(Phong_Render,"Depth");
	shader_type_loc = glGetUniformLocation(Phong_Render,"shader_type");
	Normal_Texture_Loc = glGetUniformLocation(Phong_Render,"Normal_Texture");
	cam_loc = glGetUniformLocation(Phong_Render,"cameraposition");
	linewidth_loc = glGetUniformLocation(Phong_Render, "LineWidth");
	ShaderInfo shader_info_FXAA[] =
	{
		{ GL_VERTEX_SHADER, "FXAA.vert" },
		{ GL_FRAGMENT_SHADER, "FXAA_Default.frag" },
		{ GL_NONE, NULL }
	};
	FXAA_Render = LoadShaders(shader_info_FXAA);
	glUseProgram(FXAA_Render);
	Frame_Render_Loc = glGetUniformLocation(FXAA_Render,"uSourceTex");
	rfc_loc = glGetUniformLocation(FXAA_Render,"RCPFrame");
}
void Init()
{
	vertex_list = new vmath::vec3*[1200];
	normal_list = new vmath::vec3*[1200];
	view_list = new vmath::vec3*[1200];
	for (size_t i = 0; i < 1200; i++)
	{
		vertex_list[i] = new vmath::vec3[2000];
		normal_list[i] = new vmath::vec3[2000];
		view_list[i] = new vmath::vec3[2000];
	}
	makeData();
	InitShader();
	glGenVertexArrays(1,VAO);
	glBindVertexArray(VAO[0]);
	glGenBuffers(1, VBO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO[0]);
	InitTexture();
	InitFBO();
	//AAInit();
}
void Render_Object(int type)
{
	glViewport(0, 0, WIDTH, HEIGHT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(Phong_Render);
	if (type == 4)
	{
		glUniform1i(shader_type_loc, 3);
	}
	else
	{
		glUniform1i(shader_type_loc, type);
	}
	
	glUniform3f(light_pos, lightpos[0], lightpos[1], lightpos[2]);
	glUniform4f(light_color, lightcol[0], lightcol[1], lightcol[2], lightcol[3]);
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, vmath::mat4(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1,500.0f)*vmath::lookat(cameraposition, sceneposition, up)));
	vmath::mat4 model(vmath::rotate(angle_x, angle_y,0.0f)*vmath::scale(2.2f)*vmath::translate(-5.0f, -5.0f, 0.0f));
	glUniformMatrix4fv(model_matrix, 1, GL_FALSE, model);
	glLineWidth(1.0f);
	glUniform1f(linewidth_loc,1.0f);
	for (size_t i = 1; i < vertex_list[0][0][0] + 1; i++)
	{
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(vmath::vec3)*vertex_list[i][0][0], NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vmath::vec3)*vertex_list[i][0][0], vertex_list[i][1]);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vmath::vec3)*vertex_list[i][0][0], sizeof(vmath::vec3)*vertex_list[i][0][0], normal_list[i][1]);
		glBufferSubData(GL_ARRAY_BUFFER, 2 * sizeof(vmath::vec3)*vertex_list[i][0][0], sizeof(vmath::vec3)*vertex_list[i][0][0], view_list[i][1]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vmath::vec3), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vmath::vec3), BUFFER_OFFSET(sizeof(vmath::vec3)*(int)vertex_list[i][0][0]));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(vmath::vec3), BUFFER_OFFSET(2 * sizeof(vmath::vec3)*(int)vertex_list[i][0][0]));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glBindVertexArray(VAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		
		int  fir[] = {0,1};
		int  count[] = { vertex_list[i][0][0]-1,(vertex_list[i][0][0]-1)-1};
		glMultiDrawArrays(GL_LINES,fir,count,2);
		
	//	glDrawArrays(GL_LINE_STRIP,0,vertex_list[i][0][0]-1);
		
		glBufferData(GL_ARRAY_BUFFER, 3 * (sizeof(vmath::vec3)*(vertex_list[i][0][0]-1)), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vmath::vec3)*(vertex_list[i][0][0]-1), vertex_list[i][2]);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vmath::vec3)*(vertex_list[i][0][0] - 1), sizeof(vmath::vec3)*(vertex_list[i][0][0] - 1), normal_list[i][2]);
		glBufferSubData(GL_ARRAY_BUFFER, 2 * sizeof(vmath::vec3)*(vertex_list[i][0][0] - 1), sizeof(vmath::vec3)*(vertex_list[i][0][0] - 1), view_list[i][2]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vmath::vec3), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vmath::vec3), BUFFER_OFFSET(sizeof(vmath::vec3)*((int)vertex_list[i][0][0] - 1)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(vmath::vec3), BUFFER_OFFSET(2 * sizeof(vmath::vec3)*((int)vertex_list[i][0][0] - 1)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glBindVertexArray(VAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glDrawArrays(GL_LINES, 0, vertex_list[i][0][0] / 2);
		
		
	}
	
	//glBindVertexArray(0);
	
	if (type == 4)
	{
		//vmath::vec4 *temp_texture = new vmath::vec4[WIDTH*HEIGHT];
		//glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_FLOAT, temp_texture);
		glBindTexture(GL_TEXTURE_2D, Texture_Color[0]);
		glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0,0,WIDTH,HEIGHT,0);
		float const vertexPosition[] = {
			1.0f, -1.0f,
			-1.0f, -1.0f,
			1.0f, 1.0f,
			-1.0f, 1.0f 
		};
		float const textureCoord[] = {
			1.0f, 0.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f 
		};
		glViewport(0, 0, WIDTH, HEIGHT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(FXAA_Render);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture_Color[0]);
		glUniform1i(Frame_Render_Loc, 0);
		glUniform2f(rfc_loc, float(1.0 / float(WIDTH)), float(1.0 / float(HEIGHT)));
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPosition)+sizeof(textureCoord), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexPosition), vertexPosition);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertexPosition), 0, NULL);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertexPosition), sizeof(textureCoord), textureCoord);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertexPosition)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindVertexArray(VAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	
	//glBindVertexArray(0);
}
void displayFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, DEPTHFBO[0]);
	Render_Object(1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void display() 
{
	vmath::mat4 model(vmath::rotate(angle_x, angle_y, 0.0f)*vmath::scale(2.2f)*vmath::translate(-5.0f, -5.0f, 0.0f));
	for (size_t i = 1; i <vertex_list[0][0][0] + 1; i++)
	{
		normal_list[i][0] = { vertex_list[i][0][0],0.0f,0.0f };
		view_list[i][0] = { vertex_list[i][0][0],0.0f,0.0f };
		for (size_t j = 2; j < vertex_list[i][0][0]; j++)
		{
			vmath::vec4 temp = vec4(vertex_list[i][j], 1.0f)*model;
			vec3 v_tmep;
			v_tmep = vmath::vec3(temp[0], temp[1], temp[2]);
			view_list[i][j] = cameraposition - v_tmep;
			vmath::vec4 temp1 = vec4(vertex_list[i][j + 1], 1.0f)*model;
			vec3 v_tmep1;
			v_tmep1 = vmath::vec3(temp1[0], temp1[1], temp1[2]);
			vmath::vec4 temp2 = vec4(vertex_list[i][j - 1], 1.0f)*model;
			vec3 v_tmep2;
			v_tmep2 = vmath::vec3(temp2[0], temp2[1], temp2[2]);
			normal_list[i][j] = getNormal(v_tmep2, v_tmep1, v_tmep);

		}
		vmath::vec4 temp_fir = vec4(vertex_list[i][1], 1.0f)*model;
		vec3 v_tmep_fir;
		v_tmep_fir = vmath::vec3(temp_fir[0], temp_fir[1], temp_fir[2]);
		view_list[i][1] = cameraposition - v_tmep_fir;
		vmath::vec4 temp_end = vec4(vertex_list[i][(int)vertex_list[i][0][0]], 1.0f)*model;
		vec3 v_tmep_end;
		v_tmep_end = vmath::vec3(temp_end[0], temp_end[1], temp_end[2]);
		view_list[i][(int)vertex_list[i][0][0]] = cameraposition - v_tmep_end;
		normal_list[i][1] = normal_list[i][2];
		normal_list[i][(int)vertex_list[i][0][0]] = normal_list[i][(int)vertex_list[i][0][0] - 1];
	}
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	displayFBO();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture_Depth[0]);
	glUniform1i(texture_depth_loc,0);
	glUniform3f(cam_loc,cameraposition[0],cameraposition[1],cameraposition[2]);
	//glBindFramebuffer(GL_FRAMEBUFFER, DEPTHFBO[0]);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

	Render_Object(4);
	glEnd();
	glutSwapBuffers();
}
void myReshape(int w, int h)
{
	//gluLookAt(eyeposition[0], eyeposition[1], eyeposition[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}
void Idle()
{
	display();
}
void processNormalKeys(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);
	else if (key == 'Q' || key == 'q'){
		GLfloat radius = vmath::distance(cameraposition, sceneposition);
		fRotateAngle += 0.5;

		GLfloat camX = sin(fRotateAngle) * radius + sceneposition[0];
		GLfloat camZ = cos(fRotateAngle) * radius + sceneposition[2];

		cameraposition = vmath::vec3(camX, 0.0, camZ);
	}
	else if (key == 'E' || key == 'e'){
		GLfloat radius = vmath::distance(cameraposition, sceneposition);
		fRotateAngle -= 0.5;

		GLfloat camX = sin(fRotateAngle) * radius + sceneposition[0];
		GLfloat camZ = cos(fRotateAngle) * radius + sceneposition[2];

		cameraposition = vmath::vec3(camX, 0.0, camZ);
	}
	else if (key == 'X' || key == 'x'){
		cameraposition = vmath::vec3{ max_x * 2, max_y * 2, max_z + min_z };
		sceneposition = vmath::vec3{ (max_x + min_x) / 2, (max_y + min_y) / 2, (max_z + min_z) / 2 };
		up = vmath::vec3{ 0.0f, 0.0f, 1.0f };
	}
	glutPostRedisplay();
}
void processSpecialKeys(int key, int x, int y){
	// 物体到相机的单位向量
	vmath::vec3 direction = vmath::normalize(sceneposition - cameraposition);
	// 物体到相机的单位向量 与 相机的向上向量相乘,得到垂直向量,即平移向量
	vmath::vec3 vertical = vmath::normalize(vmath::cross(direction, up));
	switch(key){
	case GLUT_KEY_LEFT:
		vertical *= 0.5;
		cameraposition += vertical;  // 移动相机位置
		sceneposition += vertical; //相机的指向位置也一起平移(不平移则以原来的目标转圈)
		break;
	case GLUT_KEY_RIGHT:
		vertical *= 0.5;
		cameraposition -= vertical;  // 移动相机位置
		sceneposition -= vertical; //相机的指向位置也一起平移(不平移则以原来的目标转圈)
		break;
	case GLUT_KEY_UP:
		direction *= 0.5;   // 移动0.2个单位向量
		cameraposition += direction;
		break;
	case GLUT_KEY_DOWN:
		direction *= 0.5;
		cameraposition -= direction;
		break;
	}
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



