//
//#include <fstream>
//#include <iostream>
//#include <sstream>
//#include"LoadShaders.h"
//#include"vmath.h"
//#include<vector>
//#define BUFFER_OFFSET(offset) ((GLvoid*) (NULL + offset))
//#define WIDTH 800
//#define HEIGHT 600
//using namespace std;
//using namespace vmath;
//const int BUFSIZE = 50 << 20;
//char Buf[BUFSIZE + 1], *buf = Buf;
//float max_x = -1, max_y = -1, max_z = -1;
//float min_x = 999999, min_y = 999999, min_z = 9999999;
//int MAX_LINE = 1200;
//int MAX_POINT = 3000;
//vmath::vec3 ** vertex_list;
//vmath::vec3 ** normal_list;
//vmath::vec3 ** view_list;
//vmath::vec3  normal_map[WIDTH*HEIGHT];
//GLuint Phong_Render;
//GLuint LineAO_Render;
//GLuint LineAO_Phong_Render;
//GLuint update_prog;
//int all_point=0;
//vmath::vec3 cameraposition;
//vmath::vec3 sceneposition;
//vmath::vec3 up;
//vmath::vec4 lightcol = {1.0f,1.0f,1.0f,1.0f};
//vmath::vec3 lightpos = { 0.4f,0.7f,-0.1f };
//GLfloat	cameraPerspective[4] = { 45.0, 1.0, 1.0, 1000.0 };
//float angle_x;
//float angle_y;
//float aspect = 800.0 / 600.0;
//GLuint light_pos;
//GLuint light_color;
//GLuint proj_loc;
//GLuint model_matrix;
//GLuint vertex_pos;
//GLuint vertex_normal;
//GLuint vertex_view;
//GLuint VAO[1];
//GLuint VBO[1];
//GLuint Texture_Depth[1];
//GLuint Texture_Color[1];
//GLuint texture_depth_loc;
//GLuint shader_type_loc;
//GLuint DEPTHFBO[1];
//GLuint COLORFBO[1];
//GLuint Texture_COLOR_FXAA[1];
//GLuint Texture_DEPTH_FXAA[1];
//GLuint Normal_Texture_Loc;
//GLuint cam_loc;
//GLuint real_render_frame;
//GLuint FXAA_Render;
//GLuint Frame_Render_Loc;
//GLuint rfc_loc;
//int old_x;
//int old_y;
//float m_aspectRatio = (float)WIDTH / (float)HEIGHT;
//bool readStreamlinesFile(char *&d) 
//{
//	for (; *buf == ' ' || *buf == '\n'; buf++);
//	if (*buf == '\0' || *buf == EOF) return false;
//	d = buf;
//	for (; *buf != ' '&&*buf != '\n'&&*buf != '\0'&&*buf != EOF; buf++);
//	*buf = '\0'; buf++;
//	return true;
//}
//vmath::vec3 getNormal(vmath::vec3 my,vmath::vec3 last) 
//{
//	vmath::vec3 tangent = last - my;
//	vmath::vec3 view = cameraposition - my;
//	vmath::vec3 vCrosst = vmath::cross(tangent,view);
//	return vmath::normalize(vmath::cross(vmath::normalize(vCrosst),tangent));
//}
//void InitConig()
//{
//	cameraposition = vmath::vec3{ max_x*2 , max_y*2,max_z+min_z };
//	sceneposition = vmath::vec3{ (max_x + min_x) / 2 ,(max_y + min_y) / 2 ,(max_z + min_z) / 2 };
//	up = vmath::vec3{ 0.0f,0.0f,1.0f };
//}
//void InitTexture()
//{
//	glGenTextures(1, Texture_Depth);
//	glBindTexture(GL_TEXTURE_2D, Texture_Depth[0]);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT,
//		0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//	glGenTextures(1,Texture_Color);
//	glBindTexture(GL_TEXTURE_2D,Texture_Color[0]);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0,
//		GL_RGBA, GL_UNSIGNED_BYTE, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	//Texture_Color[0] = generateMultiSampleTexture(4);
//	/*
//	glGenTextures(1, Texture_DEPTH_FXAA);
//	glBindTexture(GL_TEXTURE_2D, Texture_DEPTH_FXAA[0]);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT,
//		0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//	glGenTextures(1, Texture_COLOR_FXAA);
//	glBindTexture(GL_TEXTURE_2D, Texture_COLOR_FXAA[0]);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0,
//		GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	*/
//}
//void InitFBO()
//{
//	glGenFramebuffers(1, DEPTHFBO);
//	glBindFramebuffer(GL_FRAMEBUFFER, DEPTHFBO[0]);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,Texture_Color[0], 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Texture_Depth[0], 0);
//	/*
//	glGenFramebuffers(1, COLORFBO);
//	glBindFramebuffer(GL_FRAMEBUFFER, COLORFBO[0]);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture_COLOR_FXAA[0], 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Texture_DEPTH_FXAA[0], 0);
//	*/
//}
//void Mouse(int button, int state, int x, int y) //处理鼠标点击
//{
//	if (state == GLUT_DOWN) //第一次鼠标按下时,记录鼠标在窗口中的初始坐标
//		old_x = x, old_y = y;
//	//cout << "down:" << old_x << "," << old_y << endl;
//}
//void onMouseMove(int x, int y) //处理鼠标拖动
//{
//	//cout << "move:" << x << "," << y << endl;
//	angle_y += (x - old_x);
//	angle_x += (y - old_y);
//	//cout << angle_x << "," << angle_y << endl;
//	old_x = x, old_y = y;
//	glutPostRedisplay();
//}
//void makeData()
//{
//	float x, y, z;
//	char *s;
//	int line_num = 0;
//	int point_num = 0;
//	bool is_new_line = true;
//	FILE *stream;
//	stream = fopen("tornado.obj", "rb");
//	cout << "readData" << endl;
//	fread(Buf, 1, BUFSIZE, stream);
//	while (true&&line_num<600)
//	{
//		if (!readStreamlinesFile(s)) break;
//		if (is_new_line) 
//		{
//			is_new_line = false;
//			vertex_list[line_num][0] = vmath::vec3{(float)point_num, 0.0f, 0.0f};
//			point_num = 0;
//		}
//		
//		if (*(s + 1) == '\0') {
//			if (*s == 'v')
//			{
//				readStreamlinesFile(s); x = atof(s);
//				readStreamlinesFile(s); y = atof(s);
//				readStreamlinesFile(s); z = atof(s);
//				if (x > max_x)
//					max_x = x;
//				if (x < min_x)
//					min_x = x;
//				if (y > max_y)
//					max_y = y;
//				if (y < min_y)
//					min_y = y;
//				if (z > max_z)
//					max_z = z;
//				if (z < min_z)
//					min_z = z;
//				vmath::vec3 point = { x,y,z };
//				vertex_list[line_num + 1][point_num + 1] = point;
//				point_num++;
//			}
//			else if (*s == 'g' || *s == '#') 
//			{
//				is_new_line = true;
//				line_num++;
//			}
//		}
//	}
//	vertex_list[0][0] = { (float)line_num,0.0f,0.0f };
//	normal_list[0][0] = { (float)line_num,0.0f,0.0f };
//	view_list[0][0] = { (float)line_num,0.0f,0.0f };
//	InitConig();
//	vmath::mat4 model(vmath::rotate(angle_x, angle_y, 0.0f)*vmath::scale(2.2f)*vmath::translate(-5.0f, -5.0f, 0.0f));
//	for (size_t i = 1; i <vertex_list[0][0][0]; i++)
//	{
//		normal_list[i][0] = { vertex_list[i][0][0],0.0f,0.0f };
//		view_list[i][0] = { vertex_list[i][0][0],0.0f,0.0f };
//		for (size_t j = 1; j < vertex_list[i][0][0] + 1; j++)
//		{
//			vmath::vec4 temp = vec4(vertex_list[i][j], 1.0f)*model;
//			vec3 v_tmep;
//			v_tmep = vmath::vec3(temp[0], temp[1], temp[2]);
//			view_list[i][j] = cameraposition - v_tmep;
//
//			if (j<vertex_list[i][0][0])
//			{
//				vmath::vec4 temp1 = vec4(vertex_list[i][j + 1], 1.0f)*model;
//				vec3 v_tmep1;
//				v_tmep1 = vmath::vec3(temp1[0], temp1[1], temp1[2]);
//				normal_list[i][j] = getNormal(v_tmep, v_tmep1);
//			}
//			else
//			{
//				vmath::vec4 temp1 = vec4(vertex_list[i][j - 1], 1.0f)*model;
//				vec3 v_tmep1;
//				v_tmep1 = vmath::vec3(temp1[0], temp1[1], temp1[2]);
//				normal_list[i][j] = getNormal(v_tmep1, v_tmep);
//			}
//		}
//	}
//	cout << "line number:" << line_num << endl;
//}
//void InitShader()
//{
//	ShaderInfo shader_info_Phong[] =
//	{
//		{ GL_VERTEX_SHADER, "Phong.vert" },
//		{ GL_FRAGMENT_SHADER, "Phong.frag" },
//		{ GL_NONE, NULL }
//	};
//	Phong_Render = LoadShaders(shader_info_Phong);
//	glUseProgram(Phong_Render);
//	light_pos = glGetUniformLocation(Phong_Render, "lightpos");
//	light_color = glGetUniformLocation(Phong_Render, "lightcolor");
//	proj_loc = glGetUniformLocation(Phong_Render, "projection_matrix");
//	model_matrix = glGetUniformLocation(Phong_Render, "model_matrix");
//	texture_depth_loc = glGetUniformLocation(Phong_Render,"Depth");
//	shader_type_loc = glGetUniformLocation(Phong_Render,"shader_type");
//	Normal_Texture_Loc = glGetUniformLocation(Phong_Render,"Normal_Texture");
//	cam_loc = glGetUniformLocation(Phong_Render,"cameraposition");
//	ShaderInfo shader_info_FXAA[] =
//	{
//		{ GL_VERTEX_SHADER, "FXAA.vert" },
//		{ GL_FRAGMENT_SHADER, "FXAA_Default.frag" },
//		{ GL_NONE, NULL }
//	};
//	FXAA_Render = LoadShaders(shader_info_FXAA);
//	glUseProgram(FXAA_Render);
//	Frame_Render_Loc = glGetUniformLocation(FXAA_Render,"uSourceTex");
//	rfc_loc = glGetUniformLocation(FXAA_Render,"RCPFrame");
//}
//void Init()
//{
//	vertex_list = new vmath::vec3*[1200];
//	normal_list = new vmath::vec3*[1200];
//	view_list = new vmath::vec3*[1200];
//	for (size_t i = 0; i < 1200; i++)
//	{
//		vertex_list[i] = new vmath::vec3[2000];
//		normal_list[i] = new vmath::vec3[2000];
//		view_list[i] = new vmath::vec3[2000];
//	}
//	makeData();
//	InitShader();
//	glGenVertexArrays(1,VAO);
//	glBindVertexArray(VAO[0]);
//	glGenBuffers(1, VBO);
//	glBindBuffer(GL_ARRAY_BUFFER,VBO[0]);
//	InitTexture();
//	InitFBO();
//	//AAInit();
//}
//void Render_Object(int type)
//{
//	glViewport(0, 0, WIDTH, HEIGHT);
//	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glUseProgram(Phong_Render);
//	if (type == 4)
//	{
//		glUniform1i(shader_type_loc,3);
//	}
//	else
//	{
//		glUniform1i(shader_type_loc, type);
//	}
//	
//	glUniform3f(light_pos, lightpos[0], lightpos[1], lightpos[2]);
//	glUniform4f(light_color, lightcol[0], lightcol[1], lightcol[2], lightcol[3]);
//	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, vmath::mat4(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1,50.0f)*vmath::lookat(cameraposition, sceneposition, up)));
//	vmath::mat4 model(vmath::rotate(angle_x, angle_y,0.0f)*vmath::scale(2.3f)*vmath::translate(-5.0f, -5.0f, 0.0f));
//	glUniformMatrix4fv(model_matrix, 1, GL_FALSE, model);
//	glLineWidth(1.5f);
//	for (size_t i = 1; i < vertex_list[0][0][0] + 1; i++)
//	{
//		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(vmath::vec3)*vertex_list[i][0][0], NULL, GL_STATIC_DRAW);
//		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vmath::vec3)*vertex_list[i][0][0], vertex_list[i][1]);
//		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vmath::vec3)*vertex_list[i][0][0], sizeof(vmath::vec3)*vertex_list[i][0][0], normal_list[i][1]);
//		glBufferSubData(GL_ARRAY_BUFFER, 2 * sizeof(vmath::vec3)*vertex_list[i][0][0], sizeof(vmath::vec3)*vertex_list[i][0][0], view_list[i][1]);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vmath::vec3), BUFFER_OFFSET(0));
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vmath::vec3), BUFFER_OFFSET(sizeof(vmath::vec3)*(int)vertex_list[i][0][0]));
//		glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(vmath::vec3), BUFFER_OFFSET(2 * sizeof(vmath::vec3)*(int)vertex_list[i][0][0]));
//		glEnableVertexAttribArray(0);
//		glEnableVertexAttribArray(1);
//		glEnableVertexAttribArray(2);
//		glBindVertexArray(VAO[0]);
//		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
//		glDrawArrays(GL_LINE_STRIP, 0, vertex_list[i][0][0] - 1);
//	}
//	
//	//glBindVertexArray(0);
//	
//	if (type == 4)
//	{
//		//vmath::vec4 *temp_texture = new vmath::vec4[WIDTH*HEIGHT];
//		//glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_FLOAT, temp_texture);
//		glBindTexture(GL_TEXTURE_2D, Texture_Color[0]);
//		glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0,0,WIDTH,HEIGHT,0);
//		float const vertexPosition[] = {
//			1.0f, -1.0f,
//			-1.0f, -1.0f,
//			1.0f, 1.0f,
//			-1.0f, 1.0f };
//		float const textureCoord[] = {
//			1.0f, 0.0f,
//			0.0f, 0.0f,
//			1.0f, 1.0f,
//			0.0f, 1.0f };
//		glViewport(0, 0, WIDTH, HEIGHT);
//		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glUseProgram(FXAA_Render);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, Texture_Color[0]);
//		glUniform1i(Frame_Render_Loc, 0);
//		glUniform2f(rfc_loc, float(1.0 / float(WIDTH)), float(1.0 / float(HEIGHT)));
//		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPosition)+sizeof(textureCoord), NULL, GL_STATIC_DRAW);
//		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexPosition), vertexPosition);
//		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertexPosition), sizeof(textureCoord), textureCoord);
//		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertexPosition)));
//		glEnableVertexAttribArray(0);
//		glEnableVertexAttribArray(1);
//		glBindVertexArray(VAO[0]);
//		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
//		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//	}
//	
//	//glBindVertexArray(0);
//}
//void displayFBO()
//{
//	glBindFramebuffer(GL_FRAMEBUFFER, DEPTHFBO[0]);
//	Render_Object(1);
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//}
//void display() 
//{
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LEQUAL);
//	glEnable(GL_LINE_SMOOTH);
//	displayFBO();
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, Texture_Depth[0]);
//	glUniform1i(texture_depth_loc,0);
//	glUniform3f(cam_loc,cameraposition[0],cameraposition[1],cameraposition[2]);
//	//glBindFramebuffer(GL_FRAMEBUFFER, DEPTHFBO[0]);
//	Render_Object(4);
//	//glBindFramebuffer(GL_READ_FRAMEBUFFER, DEPTHFBO[0]);
//	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//	//glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
//	glFlush();
//	glutSwapBuffers();
//}
//void myReshape(int w, int h)
//{
//	//gluLookAt(eyeposition[0], eyeposition[1], eyeposition[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
//}
//void Idle()
//{
//	display();
//}
//int main(int argc, char ** argv)
//{
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GL_RGBA);
//	glutInitWindowSize(WIDTH, HEIGHT);
//	glutInitContextVersion(4, 2);
//	glutInitContextProfile(GLUT_CORE_PROFILE);
//	glutCreateWindow("first");
//	if (glewInit())
//	{
//		cerr << "Unable to initialize GLEW ... exiting" << endl;
//		exit(EXIT_FAILURE);
//	}
//	Init();
//	glutMouseFunc(Mouse);
//	glutMotionFunc(onMouseMove);
//	glutDisplayFunc(display);
//	glutReshapeFunc(myReshape);
//	glutMainLoop();
//	glutIdleFunc(Idle);
//	return 0;
//}