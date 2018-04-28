#pragma once
#include<vector>
#include<iostream>
#include"orbiter.h"
struct LightPara
{
	vmath::vec3 pos;
	vmath::vec3 nor;
	vmath::vec3 col;
	GLfloat range;
	GLint type;
};
GLfloat TempR(GLfloat r, GLfloat x, GLfloat All)
{
	GLfloat a1 = 2 * r*x / All;
	GLfloat a2 = 2 * r*(1 - x / All);
	GLfloat a3 = a1*a2;
	return pow(a3, 0.5f);
}
class SceneManager 
{
public:
	SceneManager() 
	{
	}
	void InitGameObject();
	void InitLight();
	void InitVAO();
	GLuint VAO[1];
	GLuint VBO[1];
	vmath::vec3 * pos;
	vmath::vec3 * nor;
	vmath::vec3 * col;
	float * roughness;
	int vertexcount=21660;
	LightPara lp[7];
	vmath::mat4 lp_mat[7];
	GLuint lp_count;
	vmath::vec3 cameraPos;
	vmath::vec3 scenePos;
	Transform mvp;
	Transform View;
	Transform invView;
	Transform Proj;
	Transform ViewPortProj;
	Transform invViewPortProj;
	Transform normalMatrix;
	Transform mvMatrix;
	Orbiter camera;
private:
	void SpereData(vmath::vec3*&pos,vmath::vec3*&nor,vmath::vec3*&col,vmath::vec3 center,float riaus,vmath::vec3 color,int size);
	void CubeData(vmath::vec3*&pos,vmath::vec3*&nor,vmath::vec3*&col,vmath::vec3 center,float length,vmath::vec3 color,int size);
	void planeData(vmath::vec3*&pos, vmath::vec3*&nor, vmath::vec3*&col, vmath::vec3 center, float length, vmath::vec3 nor_1,vmath::vec3 nor_2,vmath::vec3 color, int size,vmath::vec3 nor_);
	void DataLink(vmath::vec3*pos1,vmath::vec3*pos2,vmath::vec3*nor1,vmath::vec3*nor2,vmath::vec3*col1,vmath::vec3*col2,vmath::vec3*&posdes,vmath::vec3*&nordes,vmath::vec3*&coldes,int size1,int size2);
};
void SceneManager::InitVAO() 
{
	glGenVertexArrays(1, VAO);
	glBindVertexArray(VAO[0]);
	glGenBuffers(1,VBO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO[0]);
	cameraPos = vmath::vec3(0,3,-9);
	scenePos = vmath::vec3(0,0,0);
	vmath::vec3 up = vmath::vec3(0.0f, 1.0f, 0.0f);
	GLfloat aspect = 800.0f / 800.0f;
	//mvp = vmath::mat4(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1, 500.0f)*vmath::lookat(cameraPos, scenePos, up));
//	mvmat = vmath::lookat(cameraPos, scenePos, up);
//	vpmat = vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1, 500.0f);
	Point pcamera(cameraPos[0],cameraPos[1],cameraPos[2]);
	Point pscene(scenePos[0],scenePos[1],scenePos[2]);
	
	camera.lookat(pscene, 15);
	camera.translation(0.05, 0.1);
	camera.rotation(0,180,180);
	Transform M = Identity();
	Transform V = camera.view();
	Transform P = camera.projection(800.0, 600.0, 45);
	mvp = P*V*M;
	mvMatrix = V * M;
	normalMatrix = mvMatrix.normal();
	View = camera.view().normal();
	invView = camera.view().inverse();
	Proj = camera.projection(800.0, 600.0, 45);
	ViewPortProj = Viewport(800.0,600.0) * camera.projection(800.0f, 600.0f, 45);
	invViewPortProj = Viewport(800.0f, 600.0f) * camera.projection(800.0f, 600.0f, 45).inverse();
}
void SceneManager::InitLight()
{
	lp_count = 4;
	lp[0].pos = vmath::vec3(4.5,0,0);
	lp[0].nor = vmath::vec3(-0.9,0,0);
	lp[0].type = 0;
	lp[0].range = 7;
	lp[0].col = vmath::vec3(1,0,0);

	lp[1].pos = vmath::vec3(-1.5, 0, -2.5);
	lp[1].nor = vmath::vec3(0.2, 0.5, 0.9);
	lp[1].type = 0;
	lp[1].range = 3;
	lp[1].col = vmath::vec3(1, 1, 0);

	lp[2].pos = vmath::vec3(0, 2, 0);
	lp[2].nor = vmath::vec3(0.5,0.3, 0.2);
	lp[2].type = 1;
	lp[2].range = 3;
	lp[2].col = vmath::vec3(0, 0, 1);

	lp[3].pos = vmath::vec3(-1.5,1, 5);
	lp[3].nor = vmath::vec3(0, 0, -0.9);
	lp[3].type = 1;
	lp[3].range = 2;
	lp[3].col = vmath::vec3(1, 1, 1);

	lp[4].pos = vmath::vec3(-4.5, -2, 0);
	lp[4].nor = vmath::vec3(0.9, -0.4, 0);
	lp[4].type = 0;
	lp[4].range = 2;
	lp[4].col = vmath::vec3(0, 1, 1);
	for (int i = 0; i<lp_count; i++)
	{
		lp[i].pos += scenePos;
	}
	for(int i=0;i<lp_count;i++)
	{
		lp_mat[i][0] = vmath::vec4(lp[i].pos,1.0f);
		lp_mat[i][1] = vmath::vec4(lp[i].nor,1.0f);
		lp_mat[i][2] = vmath::vec4(lp[i].col,1.0f);
		lp_mat[i][3] = vmath::vec4(lp[i].range,1.0f,1.0f,1.0f);
	}
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
void SceneManager::InitGameObject()
{
	vmath::vec3 * planepos_1 = new vmath::vec3[6];
	vmath::vec3 * planepos_2 = new vmath::vec3[6];
	vmath::vec3 * planepos_3 = new vmath::vec3[6];
	vmath::vec3 * planepos_4 = new vmath::vec3[6];
	
	vmath::vec3 * planenor_1 = new vmath::vec3[6];
	vmath::vec3 * planenor_2 = new vmath::vec3[6];
	vmath::vec3 * planenor_3 = new vmath::vec3[6];
	vmath::vec3 * planenor_4 = new vmath::vec3[6];

	vmath::vec3 * planecol_1 = new vmath::vec3[6];
	vmath::vec3 * planecol_2 = new vmath::vec3[6];
	vmath::vec3 * planecol_3 = new vmath::vec3[6];
	vmath::vec3 * planecol_4 = new vmath::vec3[6];


	planeData(planepos_1,planenor_1,planecol_1,vmath::vec3(-5,0,0),10,vmath::vec3(0,1,0),vmath::vec3(0,0,1),vmath::vec3(0.2,0.2,0.2),6,vmath::vec3(1,0,0));

	planeData(planepos_2, planenor_2, planecol_2, vmath::vec3(0, 0, 5), 10, vmath::vec3(0, 1, 0), vmath::vec3(1, 0, 0), vmath::vec3(0.2, 0.2, 0.2), 6,vmath::vec3(0,0,-1));

	planeData(planepos_3, planenor_3, planecol_3, vmath::vec3(5, 0, 0), 10, vmath::vec3(0, 1, 0), vmath::vec3(0, 0, 1), vmath::vec3(0.2, 0.2, 0.2), 6,vmath::vec3(-1,0,0));
	planeData(planepos_4, planenor_4, planecol_4, vmath::vec3(0, -5, 0), 10, vmath::vec3(1, 0, 0), vmath::vec3(0, 0, 1), vmath::vec3(0.2, 0.2, 0.2), 6,vmath::vec3(0,1,0));

	vmath::vec3 * cubepos;
	vmath::vec3 * cubenor;
	vmath::vec3 * cubecol;

	CubeData(cubepos,cubenor,cubecol,vmath::vec3(-1.5,0,-3.5),1.5,vmath::vec3(0.3,0.3,0.3),36);

	vmath::vec3 * sppos_1;
	vmath::vec3 * spnor_1;
	vmath::vec3 * spcol_1;

	vmath::vec3 * sppos_2;
	vmath::vec3 * spnor_2;
	vmath::vec3 * spcol_2;

	vmath::vec3 * sppos_3;
	vmath::vec3 * spnor_3;
	vmath::vec3 * spcol_3;

	SpereData(sppos_1,spnor_1,spcol_1,vmath::vec3(1.5,-2,-1),2,vmath::vec3(0.3,0.3,0.3),2400);
	SpereData(sppos_2, spnor_2, spcol_2, vmath::vec3(2.5, 2, 2), 1.5, vmath::vec3(0.3, 0.4, 0.3), 2400);
	SpereData(sppos_3, spnor_3, spcol_3, vmath::vec3(-1.5, 3, 1), 2, vmath::vec3(0.3, 0.3, 0.3), 2400);

	vmath::vec3 * pos_1_1 = new vmath::vec3[12];
	vmath::vec3 * nor_1_1 = new vmath::vec3[12];
	vmath::vec3 * col_1_1 = new vmath::vec3[12];

	vmath::vec3 * pos_1_2 = new vmath::vec3[12];
	vmath::vec3 * nor_1_2 = new vmath::vec3[12];
	vmath::vec3 * col_1_2 = new vmath::vec3[12];

	vmath::vec3 * pos_1_3 = new vmath::vec3[7236];
	vmath::vec3 * nor_1_3 = new vmath::vec3[7236];
	vmath::vec3 * col_1_3 = new vmath::vec3[7236];

	vmath::vec3 * pos_1_4 = new vmath::vec3[14400];
	vmath::vec3 * nor_1_4 = new vmath::vec3[14400];
	vmath::vec3 * col_1_4 = new vmath::vec3[14400];

	vmath::vec3 * pos_2_1 = new vmath::vec3[24];
	vmath::vec3 * nor_2_1 = new vmath::vec3[24];
	vmath::vec3 * col_2_1 = new vmath::vec3[24];

	vmath::vec3 * pos_2_2 = new vmath::vec3[21636];
	vmath::vec3 * nor_2_2 = new vmath::vec3[21636];
	vmath::vec3 * col_2_2 = new vmath::vec3[21636];
	pos = new vmath::vec3[21660];
	nor = new vmath::vec3[21660];
	col = new vmath::vec3[21660];
	roughness = new float[21660];
	for (int i = 0; i < 21660; i++)
		roughness[i] = 0.8;

	DataLink(planepos_1,planepos_2,planenor_1,planenor_2,planecol_1,planecol_2,pos_1_1,nor_1_1,col_1_1,6,6);
	DataLink(planepos_3, planepos_4, planenor_3, planenor_4, planecol_3, planecol_4, pos_1_2, nor_1_2, col_1_2, 6, 6);
	DataLink(cubepos, sppos_1, cubenor, spnor_1, cubecol, spcol_1, pos_1_3, nor_1_3, col_1_3, 36, 7200);
	DataLink(sppos_2, sppos_3, spnor_2, spnor_3, spcol_2, spcol_3, pos_1_4, nor_1_4, col_1_4, 7200, 7200);

	DataLink(pos_1_1, pos_1_2, nor_1_1, nor_1_2, col_1_1, col_1_2, pos_2_1, nor_2_1, col_2_1, 12, 12);
	DataLink(pos_1_3, pos_1_4, nor_1_3, nor_1_4, col_1_3, col_1_4, pos_2_2, nor_2_2, col_2_2, 7236, 14400);

	DataLink(pos_2_1, pos_2_2, nor_2_1, nor_2_2, col_2_1, col_2_2, pos, nor, col, 24, 21636);
	for (int i = 0; i < vertexcount; i++)
	{
		pos[i] += scenePos;
	}
}
void SceneManager::SpereData(vmath::vec3*&pos_, vmath::vec3*&nor_, vmath::vec3*&col_, vmath::vec3 center, float riaus, vmath::vec3 color, int size) 
{
	std::vector<std::vector<vmath::vec3>> circles;
	vmath::vec3 posTop = center + vmath::vec3(0,riaus,0);
	vmath::vec3 posbottom = center + vmath::vec3(0, -riaus, 0);
	int SIZE_Z = 20;
	int SIZE_XY = 60;
	for (int i = 0; i < SIZE_Z; i++) 
	{
		float tempZ = center[1] + riaus - 2 * (riaus / (SIZE_Z+1))*(i + 1);
		std::vector<vmath::vec3> tempCircle;
		for (int j = 0; j < SIZE_XY; j++) 
		{
			float tempX = center[0] + cos((float)j / SIZE_XY * 2 * 3.1415926)*TempR(riaus,i+1,SIZE_Z+1);
			float tempY = center[2] + sin((float)j / SIZE_XY * 2 * 3.1415926)*TempR(riaus,i+1,SIZE_Z+1);
			vmath::vec3 tempPoint = vmath::vec3(tempX,tempZ,tempY);
			tempCircle.push_back(tempPoint);
		}
		circles.push_back(tempCircle);
	}
	std::vector<vmath::vec3> posSpere;
	std::vector<vmath::vec3> norSpere;
	std::vector<vmath::vec3> colSpere;
	for (int i = 0; i < SIZE_Z + 1; i++) 
	{
		if (i == 0) 
		{
			int size_ = circles[i].size();
			for (int j = 0; j < size_; j++) 
			{
				vmath::vec3 pos1 = posTop;
				vmath::vec3 pos2 = circles[i][j];
				GLint k = (j + 1) % size_;
				vmath::vec3 pos3 = circles[i][k];
				vmath::vec3 nor1 = pos1 - center;
				vmath::vec3 nor2 = pos2 - center;
				vmath::vec3 nor3 = pos3 - center;
				posSpere.push_back(pos1);
				norSpere.push_back(nor1);
				colSpere.push_back(color);
				posSpere.push_back(pos2);
				norSpere.push_back(nor2);
				colSpere.push_back(color);
				posSpere.push_back(pos3);
				norSpere.push_back(nor3);
				colSpere.push_back(color);
			}
		}
		else if (i == SIZE_Z) 
		{

			int size_ = circles[i - 1].size();
			for (int j = 0; j < size_; j++)
			{
				vmath::vec3 pos3 = posbottom;
				vmath::vec3 pos2 = circles[i-1][j];
				GLint k = (j + 1) % circles[i-1].size();
				vmath::vec3 pos1 = circles[i - 1][k];
				vmath::vec3 nor1 = pos1 - center;
				vmath::vec3 nor2 = pos2 - center;
				vmath::vec3 nor3 = pos3 - center;
				posSpere.push_back(pos1);
				norSpere.push_back(nor1);
				colSpere.push_back(color);
				posSpere.push_back(pos2);
				norSpere.push_back(nor2);
				colSpere.push_back(color);
				posSpere.push_back(pos3);
				norSpere.push_back(nor3);
				colSpere.push_back(color);
			}
		}
		else
		{
			
			for (int j = 0; j < circles.at(i).size(); j++)
			{
				vmath::vec3 pos3 = circles.at(i-1).at(j);
				vmath::vec3 pos1 = circles.at(i).at(j);
				GLint k = (j + 1) % circles.at(i).size();
				vmath::vec3 pos2 = circles.at(i).at(k);
				vmath::vec3 nor1 = pos1 - center;
				vmath::vec3 nor2 = pos2 - center;
				vmath::vec3 nor3 = pos3 - center;
				posSpere.push_back(pos1);
				norSpere.push_back(nor1);
				colSpere.push_back(color);
				posSpere.push_back(pos2);
				norSpere.push_back(nor2);
				colSpere.push_back(color);
				posSpere.push_back(pos3);
				norSpere.push_back(nor3);
				colSpere.push_back(color);
			}

			for (int j = 0; j < circles.at(i-1).size(); j++)
			{
				vmath::vec3 pos3 = circles.at(i).at((j+1)%circles.at(i-1).size());
				vmath::vec3 pos2 = circles.at(i-1).at(j);
				GLint k = (j + 1) % circles.at(i).size();
				vmath::vec3 pos1 = circles.at(i-1).at(k);
				vmath::vec3 nor1 = pos1 - center;
				vmath::vec3 nor2 = pos2 - center;
				vmath::vec3 nor3 = pos3 - center;
				posSpere.push_back(pos1);
				norSpere.push_back(nor1);
				colSpere.push_back(color);
				posSpere.push_back(pos2);
				norSpere.push_back(nor2);
				colSpere.push_back(color);
				posSpere.push_back(pos3);
				norSpere.push_back(nor3);
				colSpere.push_back(color);
			}
			
		}
	}
	pos_ = new vmath::vec3[posSpere.size()];
	nor_ = new vmath::vec3[norSpere.size()];
	col_ = new vmath::vec3[colSpere.size()];

	for (int i = 0; i < posSpere.size(); i++) 
	{
		
		pos_[i] = posSpere.at(i);
		nor_[i] = norSpere.at(i);
		col_[i] = colSpere.at(i);
	}
}
void SceneManager::CubeData(vmath::vec3*&pos_, vmath::vec3*&nor_, vmath::vec3*&col_, vmath::vec3 center, float length, vmath::vec3 color, int size) 
{
	vmath::vec3 * pos_up = new vmath::vec3[6];
	vmath::vec3 * col_up = new vmath::vec3[6];
	vmath::vec3 * nor_up = new vmath::vec3[6];

	vmath::vec3 * pos_down = new vmath::vec3[6];
	vmath::vec3 * col_down = new vmath::vec3[6];
	vmath::vec3 * nor_down = new vmath::vec3[6];

	vmath::vec3 * pos_left=new vmath::vec3[6];
	vmath::vec3 * col_left = new vmath::vec3[6];
	vmath::vec3 * nor_left = new vmath::vec3[6];

	vmath::vec3 * pos_right = new vmath::vec3[6];
	vmath::vec3 * col_right = new vmath::vec3[6];
	vmath::vec3 * nor_right = new vmath::vec3[6];

	vmath::vec3 * pos_forward = new vmath::vec3[6];
	vmath::vec3 * col_forward = new vmath::vec3[6];
	vmath::vec3 * nor_forward = new vmath::vec3[6];

	vmath::vec3 * pos_back = new vmath::vec3[6];
	vmath::vec3 * col_back = new vmath::vec3[6];
	vmath::vec3 * nor_back = new vmath::vec3[6];

	planeData(pos_up, nor_up, col_up, vmath::vec3(center[0], center[1] + length/2,center[2]),length,vmath::vec3(0,0,1),vmath::vec3(1,0,0),color,6,vmath::vec3(0,1,0));
	planeData(pos_down, nor_down, col_down, vmath::vec3(center[0], center[1] - length / 2, center[2]), length, vmath::vec3(0, 0, 1), vmath::vec3(1, 0, 0), color, 6,vmath::vec3(0,-1,0));
	planeData(pos_left, nor_left, col_left, vmath::vec3(center[0]-length/2, center[1], center[2]), length, vmath::vec3(0, 1, 0), vmath::vec3(0, 0, 1), color, 6,vmath::vec3(1,0,0));
	planeData(pos_right, nor_right, col_right, vmath::vec3(center[0] + length/2 , center[1], center[2]), length, vmath::vec3(0, 1, 0), vmath::vec3(0, 0, 1), color, 6,vmath::vec3(-1,0,0));
	planeData(pos_forward, nor_forward, col_forward, vmath::vec3(center[0], center[1], center[2]-length/2), length, vmath::vec3(1, 0, 0), vmath::vec3(0, 1, 0), color, 6,vmath::vec3(0,0,-1));
	planeData(pos_back, nor_back, col_back, vmath::vec3(center[0], center[1], center[2] + length/2), length, vmath::vec3(0, 1, 0), vmath::vec3(1, 0, 0), color, 6,vmath::vec3(0,0,1));

	vmath::vec3 * pos_up_down = new vmath::vec3[12];
	vmath::vec3 * nor_up_down = new vmath::vec3[12];
	vmath::vec3 * col_up_down = new vmath::vec3[12];

	vmath::vec3 * pos_left_right = new vmath::vec3[12];
	vmath::vec3 * nor_left_right = new vmath::vec3[12];
	vmath::vec3 * col_left_right = new vmath::vec3[12];

	vmath::vec3 * pos_forward_back = new vmath::vec3[12];
	vmath::vec3 * nor_forward_back = new vmath::vec3[12];
	vmath::vec3 * col_forward_back = new vmath::vec3[12];

	vmath::vec3 * pos_left_right_up_down = new vmath::vec3[24];
	vmath::vec3 * nor_left_right_up_down = new vmath::vec3[24];
	vmath::vec3 * col_left_right_up_down = new vmath::vec3[24];

	vmath::vec3 * pos_cube = new vmath::vec3[36];
	vmath::vec3 * nor_cube = new vmath::vec3[36];
	vmath::vec3 * col_cube = new vmath::vec3[36];

	DataLink(pos_up,pos_down,nor_up,nor_down,col_up,col_down,pos_up_down,nor_up_down,col_up_down,6,6);
	DataLink(pos_right, pos_left, nor_right, nor_left, col_right, col_left, pos_left_right, nor_left_right, col_left_right, 6, 6);
	DataLink(pos_forward, pos_back, nor_forward, nor_back, col_forward, col_back, pos_forward_back, nor_forward_back, col_forward_back, 6, 6);
	DataLink(pos_up_down, pos_left_right, nor_up_down, nor_left_right, col_up_down, col_left_right, pos_left_right_up_down, nor_left_right_up_down , col_left_right_up_down, 12,12);
	DataLink(pos_left_right_up_down, pos_forward_back, nor_left_right_up_down, nor_forward_back, col_left_right_up_down, col_forward_back, pos_cube, nor_cube, col_cube, 24, 12);
	pos_ = pos_cube;
	nor_ = nor_cube;
	col_ = col_cube;
}
void SceneManager::planeData(vmath::vec3*&pos_, vmath::vec3*&nor_, vmath::vec3*&col_, vmath::vec3 center, float length, vmath::vec3 nor_1, vmath::vec3 nor_2,vmath::vec3 color, int size,vmath::vec3 nor) 
{
	vmath::vec3  pos1 = center + nor_1*length / 2 + nor_2*length / 2;
	vmath::vec3  pos2 = center - nor_1*length / 2 + nor_2*length / 2;
	vmath::vec3  pos3 = center + nor_1*length / 2 - nor_2*length / 2;
	vmath::vec3  pos4 = center - nor_1*length / 2 - nor_2*length / 2;
	for (int i = 0; i < size; i++) 
	{
		nor_[i] = nor;
		col_[i] = color;
	}
	pos_[0] = pos1;
	pos_[1] = pos2;
	pos_[2] = pos3;
	pos_[3] = pos4;
	pos_[4] = pos3;
	pos_[5] = pos2;
}
void SceneManager::DataLink(vmath::vec3*pos1, vmath::vec3*pos2, vmath::vec3*nor1, vmath::vec3*nor2, vmath::vec3*col1, vmath::vec3*col2, vmath::vec3*&posdes, vmath::vec3*&nordes, vmath::vec3*&coldes,int size1,int size2) 
{
	for (int i = 0; i < size1; i++) 
	{

		posdes[i] = pos1[i];
		nordes[i] = nor1[i];
		coldes[i] = col1[i];
	}

	for (int i = 0; i < size2; i++)
	{
		posdes[i+size1] = pos2[i];
		nordes[i+size1] = nor2[i];
		coldes[i+size1] = col2[i];
	}
}