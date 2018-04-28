
#include"LoadShaders.h"
#include"vmath.h"
#include <time.h>
#include<iostream>
#define BUFFER_OFFSET(offset) ((GLvoid*) (NULL + offset))
#define LIGHTRADIO_VERTEX_SHADER "lightradio.vert"
#define LIGHTRADIO_FRAAGMENT_SHADER "lightradio.frag"
#define VOLUMETRICLIGHT_VERTEX_SHADER "volumetriclight.vert"
#define VOLUMETRICLIGHT_FRAGMENT_SHADER "volumetriclight.frag"
#define RENDERFORGAMEOBJECT_VERTEX_SHADER "renderforgameobject.vert"
#define RENDERFORGAMEOBJECT_FRAGMENT_SHADER "renderforgameobject.frag"
#define FBO_MASK 2
#define FBO_MAIN 1
#define FBO_LIGHTRADIO 0 
#define TEXTURE_MASK 2
#define TEXTURE_MAIN 1
#define TEXTURE_LIGHTRADIO 0 
#define LEFT_UP_FORWARD 0
#define RIGHT_UP_FORWARD 1
#define LEFT_DOWN_FORWARD 2
#define RIGHT_DOWN_FORWARD 3
#define LEFT_UP_BACK 4
#define RIGHT_UP_BACK 5
#define LEFT_DOWN_BACK 6
#define RIGHT_DOWN_BACK 7
#define DEBUG_MASK
vmath::vec3 vPoint[8];
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
	GLuint maxRandomHeight = 16;
	GLuint totalSize = 40;
	GLuint FBO[3];
	GLuint FBO_ZBuffer[3];
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

struct Rect
{
	vmath::vec3 color;
	GLfloat weight;
	GLfloat height;
	vmath::vec3 center;
};

GLobalParameter globalpara;
Rect * GameObjectList;

int rectcount = 0;
vmath::vec2 lightpos = vmath::vec2(400,300);;
GLfloat getRandData(int min, int max)
{
	GLfloat m1 = (GLfloat)(rand() % 101) / 101;
	min++;                                                                           
	GLfloat m2 = (GLfloat)((rand() % (max - min + 1)) + min);
	m2 = m2 - 1;                                                                      
	return m1 + m2;                                                                
}

Rect randomRect() 
{
	Rect r;
	r.weight = getRandData(1,globalpara.maxRandomSize);
	r.height = getRandData(2, globalpara.maxRandomHeight);
	r.center[0] = getRandData(r.weight/2,globalpara.totalSize-r.weight/2);
	r.center[2] = getRandData(r.weight/2,globalpara.totalSize-r.weight/2);
	r.center[1] = r.height / 2;
	r.color = vmath::vec3(getRandData(0,1),getRandData(0,1),getRandData(0,1));
	return r;
}
vmath::vec2 lightcoord()
{
	GLfloat aspect = 800.0f / 600.0f;
	vmath::vec3 up = vmath::vec3(0.0f, 1.0f, 0.0f);
	vmath::vec3 sceneposition = vmath::vec3(20.0f, 4.0f, 20.0f);
	vmath::vec4 lightScreen = vmath::vec4(globalpara.LightPos[0], globalpara.LightPos[1], globalpara.LightPos[2], 1.0f)*vmath::lookat(globalpara.viewpoint, sceneposition, up);
	lightScreen = lightScreen*vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1, 500.0f);
	vmath::vec2 LightCoord = vmath::vec2(400.0f, 350.0f);
	return lightpos;
}
void Init() 
{
	vPoint[0] = vmath::vec3(-0.5,0.5,-0.5);
	vPoint[1] = vmath::vec3(0.5,0.5,-0.5);
	vPoint[2] = vmath::vec3(-0.5,-0.5,-0.5);
	vPoint[3] = vmath::vec3(0.5,-0.5,-0.5);
	vPoint[4] = vmath::vec3(-0.5, 0.5, 0.5);
	vPoint[5] = vmath::vec3(0.5, 0.5, 0.5);
	vPoint[6] = vmath::vec3(-0.5, -0.5, 0.5);
	vPoint[7] = vmath::vec3(0.5, -0.5, 0.5);
	int count = getRandData(10,globalpara.maxRandomCount);
	rectcount = count;
	GameObjectList = new Rect[count];
	for (int i = 0; i < count; i++) 
	{
		GameObjectList[i] = randomRect();
		//std::cout << GameObjectList[i].center[0]<<" "<< GameObjectList[i].center[1]<<" "<< GameObjectList[i].center[2]<< std::endl;
	}
	globalpara.LightDir = vmath::vec3(-0.4f,0.3f,-0.6f);
	globalpara.LightPos = vmath::vec3(20,6,40);
	globalpara.viewpoint = vmath::vec3(20,4,-8);
	globalpara.backgroundColor = vmath::vec3(0,0,1);
	globalpara.Decay = 1.0f;
	globalpara.Exposure = 0.8;
	globalpara.Density = 1.0;
	globalpara.weight = 1/128.0f;
	globalpara.maxRadioDistance = 38;
	globalpara.NUM_SAMPLES = 128;

	glGenTextures(3,globalpara.texture);
	for (int i = 0; i < 3; i++) 
	{
		glBindTexture(GL_TEXTURE_2D, globalpara.texture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600,
			0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glGenRenderbuffers(3, globalpara.FBO_ZBuffer);
	for (int i = 0; i < 3; i++) 
	{
		glBindRenderbuffer(GL_RENDERBUFFER, globalpara.FBO_ZBuffer[i]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
			800, 600);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	glGenVertexArrays(1, globalpara.VAO);
	glBindVertexArray(globalpara.VAO[0]);
	glGenBuffers(1, globalpara.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, globalpara.VBO[0]);
	glGenFramebuffersEXT(3, globalpara.FBO);
	
	for (int i = 0; i < 3; i++) 
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, globalpara.FBO[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, globalpara.texture[i], 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
			GL_RENDERBUFFER, globalpara.FBO_ZBuffer[i]);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
	ShaderInfo shaderinfo_lightradio[] = 
	{
		{GL_VERTEX_SHADER,LIGHTRADIO_VERTEX_SHADER},
		{GL_FRAGMENT_SHADER,LIGHTRADIO_FRAAGMENT_SHADER},
		{GL_NONE,NULL}
	};

	ShaderInfo shaderinfo_volumetriclight[] =
	{
		{ GL_VERTEX_SHADER,VOLUMETRICLIGHT_VERTEX_SHADER },
		{ GL_FRAGMENT_SHADER,VOLUMETRICLIGHT_FRAGMENT_SHADER },
		{ GL_NONE,NULL }
	};

	ShaderInfo shaderinfo_renderforgameobject[] =
	{
		{ GL_VERTEX_SHADER,RENDERFORGAMEOBJECT_VERTEX_SHADER },
		{ GL_FRAGMENT_SHADER,RENDERFORGAMEOBJECT_FRAGMENT_SHADER },
		{ GL_NONE,NULL }
	};

	globalpara.lightradio_shader=LoadShaders(shaderinfo_lightradio);
	glUseProgram(globalpara.lightradio_shader);
	globalpara.lightradio_lightpos_loc = glGetUniformLocation(globalpara.lightradio_shader,"lightpos");
	globalpara.lightradio_mask_loc = glGetUniformLocation(globalpara.lightradio_shader, "texture_mask");
	globalpara.lightradio_maxDis_loc = glGetUniformLocation(globalpara.lightradio_shader, "maxDistance");

	globalpara.volumetriclight_shader = LoadShaders(shaderinfo_volumetriclight);
	glUseProgram(globalpara.volumetriclight_shader);
	globalpara.volumetriclight_decay_loc = glGetUniformLocation(globalpara.volumetriclight_shader, "Decay");
	globalpara.volumetriclight_density_loc = glGetUniformLocation(globalpara.volumetriclight_shader, "Density");
	globalpara.volumetriclight_exposure_loc = glGetUniformLocation(globalpara.volumetriclight_shader, "Exposure");
	globalpara.volumetriclight_lightcoord_loc = glGetUniformLocation(globalpara.volumetriclight_shader, "lightcoord");
	globalpara.volumetriclight_lightradiotex_loc = glGetUniformLocation(globalpara.volumetriclight_shader, "texture_lightradio");
	globalpara.volumetriclight_maintex_loc = glGetUniformLocation(globalpara.volumetriclight_shader, "texture_main");
	globalpara.volumetriclight_numsamples_loc = glGetUniformLocation(globalpara.volumetriclight_shader, "NUM_SAMPLES");
	globalpara.volumetriclight_weight_loc = glGetUniformLocation(globalpara.volumetriclight_shader, "weight");

	globalpara.renderforgameobject_shader = LoadShaders(shaderinfo_renderforgameobject);
	glUseProgram(globalpara.renderforgameobject_shader);
	globalpara.renderforgameobject_lightdir_loc = glGetUniformLocation(globalpara.renderforgameobject_shader, "lightdir");
	globalpara.renderforgameobject_projmat_loc = glGetUniformLocation(globalpara.renderforgameobject_shader, "proj_mat");
	globalpara.renderforgameobject_type_loc = glGetUniformLocation(globalpara.renderforgameobject_shader, "type");

}

void RenderForLightRadio() 
{
	glViewport(0, 0, 800, 600);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(globalpara.lightradio_shader);
	glUniform2f(globalpara.lightradio_lightpos_loc,lightcoord()[0],lightcoord()[1]);
	glUniform3f(globalpara.lightradio_lightpos_loc, globalpara.LightPos[0], globalpara.LightPos[1], globalpara.LightPos[2]);
	glUniform1f(globalpara.lightradio_maxDis_loc, globalpara.maxRadioDistance);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, globalpara.texture[TEXTURE_MASK]);
	glUniform1i(globalpara.lightradio_mask_loc,0);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPosition) + sizeof(textureCoord), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexPosition), vertexPosition);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertexPosition), sizeof(textureCoord), textureCoord);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertexPosition)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(globalpara.VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, globalpara.VBO[0]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

vmath::vec3* normalForRect(Rect r) 
{
	vmath::vec3* normalall = new vmath::vec3[30];
	for (int i = 0; i < 6; i++) 
	{
		normalall[i] = vmath::vec3(0,0,-1);
		normalall[i + 6] = vmath::vec3(1, 0, 0);
		normalall[i + 12] = vmath::vec3(0,0,1);
		normalall[i + 18] = vmath::vec3(-1,0,0);
		normalall[i + 24] = vmath::vec3(0,1,0);
	}
	return normalall;
}

vmath::vec3* positionForRect(Rect r)
{
	vmath::vec3 * positionall = new vmath::vec3[30];
	vmath::vec3 center = r.center;
	vmath::vec3 delta = vmath::vec3(r.weight,r.height,r.weight);

	positionall[0] = center + delta*vPoint[LEFT_UP_FORWARD];
	positionall[1] = center + delta*vPoint[LEFT_DOWN_FORWARD];
	positionall[2] = center + delta*vPoint[RIGHT_UP_FORWARD];

	positionall[3] = center + delta*vPoint[RIGHT_UP_FORWARD];
	positionall[4] = center + delta*vPoint[RIGHT_DOWN_FORWARD];
	positionall[5] = center + delta*vPoint[LEFT_DOWN_FORWARD];

	positionall[6] = center + delta*vPoint[RIGHT_UP_FORWARD];
	positionall[7] = center + delta*vPoint[RIGHT_DOWN_FORWARD];
	positionall[8] = center + delta*vPoint[RIGHT_DOWN_BACK];

	positionall[9] = center + delta*vPoint[RIGHT_UP_FORWARD];
	positionall[10] = center + delta*vPoint[RIGHT_UP_BACK];
	positionall[11] = center + delta*vPoint[RIGHT_DOWN_BACK];

	positionall[12] = center + delta*vPoint[LEFT_UP_BACK];
	positionall[13] = center + delta*vPoint[LEFT_DOWN_BACK];
	positionall[14] = center + delta*vPoint[RIGHT_UP_BACK];

	positionall[15] = center + delta*vPoint[RIGHT_UP_BACK];
	positionall[16] = center + delta*vPoint[RIGHT_DOWN_BACK];
	positionall[17] = center + delta*vPoint[LEFT_DOWN_BACK];

	positionall[18] = center + delta*vPoint[LEFT_UP_FORWARD];
	positionall[19] = center + delta*vPoint[LEFT_DOWN_FORWARD];
	positionall[20] = center + delta*vPoint[LEFT_DOWN_BACK];

	positionall[21] = center + delta*vPoint[LEFT_UP_FORWARD];
	positionall[22] = center + delta*vPoint[LEFT_UP_BACK];
	positionall[23] = center + delta*vPoint[LEFT_DOWN_BACK];

	positionall[24] = center + delta*vPoint[LEFT_UP_FORWARD];
	positionall[25] = center + delta*vPoint[LEFT_UP_BACK];
	positionall[26] = center + delta*vPoint[RIGHT_UP_BACK];

	positionall[27] = center + delta*vPoint[RIGHT_UP_BACK];
	positionall[28] = center + delta*vPoint[RIGHT_UP_FORWARD];
	positionall[29] = center + delta*vPoint[LEFT_UP_FORWARD];

	return positionall;
	//positionall[0] = 
}

vmath::vec3* colorForRect(Rect r)
{
	vmath::vec3 color = r.color;
	vmath::vec3* colorall = new vmath::vec3[30];
	for (int i = 0; i < 30; i++) 
	{
		colorall[i] = color;
	}
	return colorall;
}



void RenderForGameObject(int type) 
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glViewport(0, 0, 800, 600);
	if (type == 0) 
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		glClearColor(globalpara.backgroundColor[0], globalpara.backgroundColor[1], globalpara.backgroundColor[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	glUseProgram(globalpara.renderforgameobject_shader);
	glUniform1i(globalpara.renderforgameobject_type_loc, type);
	glUniform3f(globalpara.renderforgameobject_lightdir_loc,globalpara.LightDir[0], globalpara.LightDir[1], globalpara.LightDir[2]);
	GLfloat aspect = 800.0f / 600.0f;
	vmath::vec3 up = vmath::vec3(0.0f,1.0f,0.0f);
	vmath::vec3 sceneposition = vmath::vec3(20.0f,4.0f,20.0f);
	glUniformMatrix4fv(globalpara.renderforgameobject_projmat_loc, 1, GL_FALSE, vmath::mat4(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1, 500.0f)*vmath::lookat(globalpara.viewpoint, sceneposition, up)));
//	std::cout << rectcount << std::endl;
	for(int i=0;i<rectcount;i++)
	{
		glBufferData(GL_ARRAY_BUFFER,sizeof(vmath::vec3)*3*10*3,NULL,GL_STATIC_DRAW);
		vmath::vec3 * pos = positionForRect(GameObjectList[i]);
		vmath::vec3 * nor = normalForRect(GameObjectList[i]);
		vmath::vec3 * col = colorForRect(GameObjectList[i]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vmath::vec3) * 30, pos[0]);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vmath::vec3) * 30, sizeof(vmath::vec3) * 30, nor[0]);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vmath::vec3) * 30*2, sizeof(vmath::vec3) * 30, col[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vmath::vec3), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vmath::vec3), BUFFER_OFFSET(sizeof(vmath::vec3)*30));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vmath::vec3), BUFFER_OFFSET(sizeof(vmath::vec3)*60));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glBindVertexArray(globalpara.VAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, globalpara.VBO[0]);
		glDrawArrays(GL_TRIANGLES,0,30);
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(vmath::vec3) * 3 * 6, NULL, GL_STATIC_DRAW);
	vmath::vec3 pos_plane[6] = {vmath::vec3(0,0,0),vmath::vec3(0,0,40),vmath::vec3(40,0,0),
								vmath::vec3(40,0,0),vmath::vec3(40,0,40),vmath::vec3(0,0,40) };
	vmath::vec3 nor_plane[6] = {vmath::vec3(0,1,0),vmath::vec3(0,1,0),vmath::vec3(0,1,0),
								vmath::vec3(0,1,0),vmath::vec3(0,1,0),vmath::vec3(0,1,0) };
	vmath::vec3 color_plane[6] = { vmath::vec3(0.3,0.6,0.6),vmath::vec3(0.3,0.6,0.6),vmath::vec3(0.3,0.6,0.6),
		vmath::vec3(0.3,0.6,0.6),vmath::vec3(0.3,0.6,0.6),vmath::vec3(0.3,0.6,0.6) };
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vmath::vec3) * 6, pos_plane[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vmath::vec3) * 6, sizeof(vmath::vec3) * 6, nor_plane[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vmath::vec3) * 6 * 2, sizeof(vmath::vec3) * 6, color_plane[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vmath::vec3), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vmath::vec3), BUFFER_OFFSET(sizeof(vmath::vec3) * 6));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vmath::vec3), BUFFER_OFFSET(sizeof(vmath::vec3) * 12));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindVertexArray(globalpara.VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, globalpara.VBO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnd();
}

void Display() 
{
	//RenderForGameObject(1);
	//glutSwapBuffers();
	//return;
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, globalpara.FBO[FBO_MAIN]);

	RenderForGameObject(1);
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, globalpara.FBO[FBO_MASK]);
	RenderForGameObject(0);
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, globalpara.FBO[FBO_LIGHTRADIO]);
	RenderForLightRadio();
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
	//glBindTexture(GL_TEXTURE_2D, globalpara.texture[TEXTURE_MAIN]);
	//glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 800, 600, 0);
	glViewport(0, 0, 800, 600);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(globalpara.volumetriclight_shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, globalpara.texture[TEXTURE_MAIN]);
	glUniform1i(globalpara.volumetriclight_maintex_loc, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, globalpara.texture[TEXTURE_LIGHTRADIO]);
	glUniform1i(globalpara.volumetriclight_lightradiotex_loc, 1);
	glUniform1f(globalpara.volumetriclight_numsamples_loc, globalpara.NUM_SAMPLES);
	glUniform1f(globalpara.volumetriclight_decay_loc, globalpara.Decay);
	glUniform1f(globalpara.volumetriclight_weight_loc, globalpara.weight);
	glUniform1f(globalpara.volumetriclight_density_loc, globalpara.Density);
	glUniform1f(globalpara.volumetriclight_exposure_loc, globalpara.Exposure);
	glUniform2f(globalpara.volumetriclight_lightcoord_loc, lightcoord()[0],lightcoord()[1]);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPosition) + sizeof(textureCoord), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexPosition), vertexPosition);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertexPosition), sizeof(textureCoord), textureCoord);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertexPosition)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(globalpara.VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, globalpara.VBO[0]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glutSwapBuffers();
	
}

void Idle() 
{
	Display();
}
void processSpecialKeys(int key, int x, int y)
{

	switch (key)
	{
	case GLUT_KEY_UP: lightpos[1] += 2; break;
	case GLUT_KEY_DOWN: lightpos[1] -= 2; break;
	case GLUT_KEY_RIGHT: lightpos[0] += 2; break;
	case GLUT_KEY_LEFT: lightpos[0] -= 2; break;
	}
}
void myReshape(int w, int h)
{
	//gluLookAt(eyeposition[0], eyeposition[1], eyeposition[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}
int main(int argc, char ** argv)
{	
	srand((unsigned)time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Volumetric Light Demo");
	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	if (glewInit())
	{
		exit(EXIT_FAILURE);
	}
	Init();

	glutReshapeFunc(myReshape);
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutSpecialFunc(processSpecialKeys);
	glutMainLoop();
	
	return 0;
}