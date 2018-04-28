#pragma once
#include"glConfigruation.h"
#include"PBALRenderer.h"

#define N 4
enum TEXTURETYPE:unsigned int
{

	MAINTEX = 0,
	DEPTHNORMALSTEX,
	CULLBACKTEX,
	CULLFRONTTEX,
	POSTEX,
	THICKNESSTEX
};
enum  FRAMEBUFFERTYPE : unsigned int 
{
	MAINTEXFRAMEBUFFER = 0,
	DEPTHNORMALSTEXFRAMMEBUFFER ,
	CULLBACKTEXFRAMEBUFFER , 
	CULLFRONTDFRAMEBUFFER ,
	POSTEXFRAMEBUFFER
};
//按第一行展开计算|A|
double getA(double arcs[N][N], int n)
{
	if (n == 1)
	{
		return arcs[0][0];
	}
	double ans = 0;
	double temp[N][N] = { 0.0 };
	int i, j, k;
	for (i = 0; i<n; i++)
	{
		for (j = 0; j<n - 1; j++)
		{
			for (k = 0; k<n - 1; k++)
			{
				temp[j][k] = arcs[j + 1][(k >= i) ? k + 1 : k];

			}
		}
		double t = getA(temp, n - 1);
		if (i % 2 == 0)
		{
			ans += arcs[0][i] * t;
		}
		else
		{
			ans -= arcs[0][i] * t;
		}
	}
	return ans;
}
//计算每一行每一列的每个元素所对应的余子式，组成A*
void  getAStart(double arcs[N][N], int n, double ans[N][N])
{
	if (n == 1)
	{
		
		ans[0][0] = 1;
		return;
	}
	int i, j, k, t;
	double temp[N][N];
	for (i = 0; i<n; i++)
	{
		for (j = 0; j<n; j++)
		{
			for (k = 0; k<n - 1; k++)
			{
				for (t = 0; t<n - 1; t++)
				{
					temp[k][t] = arcs[k >= i ? k + 1 : k][t >= j ? t + 1 : t];
				}
			}


			ans[j][i] = getA(temp, n - 1);
			if ((i + j) % 2 == 1)
			{
				ans[j][i] = -ans[j][i];
			}
		}
	}
}
bool GetMatrixInverse(double src[N][N], int n, double des[N][N])
{
	double flag = getA(src, n);
	double t[N][N];
	if (flag == 0)
	{
		return false;
	}
	else
	{
		getAStart(src, n, t);
		for (int i = 0; i<n; i++)
		{
			for (int j = 0; j<n; j++)
			{
				des[i][j] = t[i][j] / flag;
			}

		}
	}


	return true;

}
class SSREffect
{
public:
	void preprocess(SceneManager *sm,PBALRenderer * pr);
	void Render(SceneManager * sm);
	void Init(SceneManager * sm);
private:
	vmath::mat4 inversion(vmath::mat4 m);
	vmath::mat4 WorldToView;
	vmath::mat4 ViewToWorld;
	float rayLength=10;

	GLuint allTex[6];
	GLuint allFrameBuffer[5];
	GLuint allRenderBuffer[5];
	GLuint DepthTex[1];

	GLuint SSR_Shader;
	GLuint DepthNormalsTex_Shader;
	GLuint Cull_Shader;
	GLuint pos_shader;

	GLuint mvp_cull_loc;
	GLuint viewpoint_loc;
	GLuint RayLength_loc;
	GLuint MainTex_loc;
	GLuint thicknessTex_loc;
	GLuint depthNormalsTex_loc;
	GLuint mvp_loc;
	GLuint mvp_pos_loc;
	GLuint posTex_loc;
	GLuint mvMatrix_loc;
	GLuint normalMatrix_loc;
	GLuint ssr_view_loc;
	GLuint ssr_invview_loc;
	GLuint ssr_proj_loc;
	GLuint ssr_viewportproj_loc;
	GLuint ssr_invviewportproj_loc;
	vmath::vec4 * thicknessTexArray;
};
void SSREffect::Init(SceneManager *sm)
{
	thicknessTexArray = new vmath::vec4[800 * 600];

	ShaderInfo si_ssr[] = { {GL_VERTEX_SHADER,SSR_VERT_SHADER},{GL_FRAGMENT_SHADER,SSR_FRAG_SHADER},{GL_NONE,NULL} };
	ShaderInfo si_dn[] = { {GL_VERTEX_SHADER,DEPTHNORMALSTEX_VERT_SHADER},{GL_FRAGMENT_SHADER,DEPTHNORMALSTEX_FRAG_SHADER},{GL_NONE,NULL} };
	ShaderInfo si_cull[] = { {GL_VERTEX_SHADER,CULL_VERT_SHADER},{GL_FRAGMENT_SHADER,CULL_FRAG_SHADER},{GL_NONE,NULL} };
	ShaderInfo si_pos[] = { { GL_VERTEX_SHADER,POS_VERT_SHADER },{ GL_FRAGMENT_SHADER,POS_FRAG_SHADER },{ GL_NONE,NULL } };

	SSR_Shader = LoadShaders(si_ssr);
	DepthNormalsTex_Shader = LoadShaders(si_dn);
	Cull_Shader = LoadShaders(si_cull);
	pos_shader = LoadShaders(si_pos);
	glGenTextures(6, allTex);
	for (int i = 0; i < 6; i++) 
	{
		glBindTexture(GL_TEXTURE_2D, allTex[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600,0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glGenTextures(1, DepthTex);
	glBindTexture(GL_TEXTURE_2D, DepthTex[0]);

	glTexImage2D(GL_TEXTURE_2D, 0,
		GL_DEPTH_COMPONENT, 800, 600, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenRenderbuffers(5, allRenderBuffer);
	for (int i = 0; i < 5; i++)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, allRenderBuffer[i]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
			800, 600);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	glGenFramebuffers(5,allFrameBuffer);
	for (int i = 0; i < 5; i++)
	{
		if (i != DEPTHNORMALSTEXFRAMMEBUFFER) 
		{
			glBindFramebuffer(GL_FRAMEBUFFER, allFrameBuffer[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, allTex[i], 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				GL_RENDERBUFFER, allRenderBuffer[i]);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, allFrameBuffer[DEPTHNORMALSTEXFRAMMEBUFFER]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, allTex[DEPTHNORMALSTEXFRAMMEBUFFER], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTex[0], 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	mvp_cull_loc = glGetUniformLocation(Cull_Shader,"mvp");
	MainTex_loc = glGetUniformLocation(SSR_Shader,"MainTex");
	thicknessTex_loc = glGetUniformLocation(SSR_Shader, "DepthTex");
	depthNormalsTex_loc = glGetUniformLocation(SSR_Shader,"depthNormalsTex");
	mvp_loc = glGetUniformLocation(DepthNormalsTex_Shader, "mvp");
	mvp_pos_loc = glGetUniformLocation(pos_shader,"mvp");
	posTex_loc = glGetUniformLocation(SSR_Shader,"posTex");
	mvMatrix_loc = glGetUniformLocation(pos_shader,"mvMatrix");
	normalMatrix_loc = glGetUniformLocation(DepthNormalsTex_Shader,"normalMatrix");
	ssr_view_loc = glGetUniformLocation(SSR_Shader,"View");
	ssr_invview_loc = glGetUniformLocation(SSR_Shader, "invView");
	ssr_proj_loc = glGetUniformLocation(SSR_Shader, "Proj");
	ssr_viewportproj_loc = glGetUniformLocation(SSR_Shader, "ViewPortProj");
	ssr_invviewportproj_loc = glGetUniformLocation(SSR_Shader, "invViewPortProj");
}

void SSREffect::Render(SceneManager * sm) 
{
	glViewport(0, 0, 800, 600);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(SSR_Shader);

	glUniformMatrix4fv(ssr_view_loc,1,GL_TRUE,sm->View.buffer());
	glUniformMatrix4fv(ssr_invview_loc, 1, GL_TRUE, sm->invView.buffer());
	glUniformMatrix4fv(ssr_proj_loc, 1, GL_TRUE, sm->Proj.buffer());
	glUniformMatrix4fv(ssr_viewportproj_loc, 1, GL_TRUE, sm->ViewPortProj.buffer());
	glUniformMatrix4fv(ssr_invviewportproj_loc, 1, GL_TRUE, sm->invViewPortProj.buffer());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, allTex[DEPTHNORMALSTEX]);
	glUniform1i(depthNormalsTex_loc, 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, allTex[POSTEX]);
	glUniform1i(posTex_loc,2);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, DepthTex[0]);
	glUniform1i(thicknessTex_loc, 1);


	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, allTex[MAINTEX]);
	glUniform1i(MainTex_loc, 3);

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
	glBindVertexArray(sm->VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, sm->VBO[0]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glutSwapBuffers();
}

void SSREffect::preprocess(SceneManager *sm, PBALRenderer *pr) 
{

	
	glBindFramebuffer(GL_FRAMEBUFFER,allFrameBuffer[MAINTEXFRAMEBUFFER]);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glViewport(0, 0, 800, 600);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	pr->Render(sm);
	//glutSwapBuffers();
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	
	glBindFramebuffer(GL_FRAMEBUFFER, allFrameBuffer[DEPTHNORMALSTEXFRAMMEBUFFER]);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glViewport(0, 0, 800, 600);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(DepthNormalsTex_Shader);
	glUniform3f(viewpoint_loc,sm->cameraPos[0],sm->cameraPos[1],sm->cameraPos[2]);
	glUniformMatrix4fv(mvp_loc,1,GL_TRUE,sm->mvp.buffer());
	glUniformMatrix4fv(normalMatrix_loc,1,GL_TRUE,sm->normalMatrix.buffer());
	vmath::vec3 * pos = sm->pos;
	vmath::vec3 * nor = sm->nor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(vmath::vec3) * 2 * sm->vertexcount, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vmath::vec3) * sm->vertexcount, pos[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vmath::vec3) * sm->vertexcount, sizeof(vmath::vec3) * sm->vertexcount, nor[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vmath::vec3), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vmath::vec3), BUFFER_OFFSET(sizeof(vmath::vec3) * sm->vertexcount));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(sm->VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, sm->VBO[0]);
	glDrawArrays(GL_TRIANGLES, 0, sm->vertexcount);
	glEnd();
	glutSwapBuffers();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, allFrameBuffer[CULLBACKTEXFRAMEBUFFER]);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glViewport(0, 0, 800, 600);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(Cull_Shader);
	glUniformMatrix4fv(mvp_cull_loc, 1, GL_TRUE, sm->mvp.buffer());
	glBufferData(GL_ARRAY_BUFFER, sizeof(vmath::vec3) * 2 * (sm->vertexcount-24), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vmath::vec3) * (sm->vertexcount-24), pos[24]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vmath::vec3) * (sm->vertexcount-24), sizeof(vmath::vec3) * (sm->vertexcount-24), nor[24]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vmath::vec3), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vmath::vec3), BUFFER_OFFSET(sizeof(vmath::vec3) * (sm->vertexcount-24)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(sm->VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, sm->VBO[0]);
	glDrawArrays(GL_TRIANGLES, 0, sm->vertexcount-24);
	glEnd();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, allFrameBuffer[CULLFRONTDFRAMEBUFFER]);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glViewport(0, 0, 800, 600);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(Cull_Shader);
	glUniformMatrix4fv(mvp_cull_loc, 1, GL_TRUE, sm->mvp.buffer());
	glBufferData(GL_ARRAY_BUFFER, sizeof(vmath::vec3) * 2 * (sm->vertexcount-24), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vmath::vec3) * (sm->vertexcount-24), pos[24]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vmath::vec3) * (sm->vertexcount-24), sizeof(vmath::vec3) * (sm->vertexcount-24), nor[24]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vmath::vec3), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vmath::vec3), BUFFER_OFFSET(sizeof(vmath::vec3) * (sm->vertexcount-24)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(sm->VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, sm->VBO[0]);
	glDrawArrays(GL_TRIANGLES, 0, sm->vertexcount-24);
	glEnd();	
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	vmath::vec4 *cullfrontDepth = new vmath::vec4[800*600];
	vmath::vec4 *cullbackDepth = new vmath::vec4[800 * 600];
	glBindTexture(GL_TEXTURE_2D,allTex[CULLBACKTEX]);
	glGetTexImage(GL_TEXTURE_2D,0, GL_RGBA,GL_FLOAT, cullbackDepth);
	glBindTexture(GL_TEXTURE_2D, allTex[CULLFRONTTEX]);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, cullfrontDepth);
	
	for (int i = 0; i < 800; i++)
		for (int j = 0; j < 600; j++) 
		{
			thicknessTexArray[i * 600 + j] = vmath::vec4(cullbackDepth[i * 600 + j][0] - cullfrontDepth[i * 600 + j][0], 0, 0, 0);
		}
	glBindTexture(GL_TEXTURE_2D,allTex[THICKNESSTEX]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_FLOAT, thicknessTexArray);
	
	glBindFramebuffer(GL_FRAMEBUFFER, allFrameBuffer[POSTEXFRAMEBUFFER]);
	glDisable(GL_CULL_FACE);
	glViewport(0, 0, 800, 600);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(pos_shader);
	glUniformMatrix4fv(mvp_pos_loc, 1, GL_TRUE, sm->mvp.buffer());
	glUniformMatrix4fv(mvMatrix_loc,1,GL_TRUE,sm->mvMatrix.buffer());
	glBufferData(GL_ARRAY_BUFFER, sizeof(vmath::vec3)* sm->vertexcount, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vmath::vec3) * sm->vertexcount, pos[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vmath::vec3), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindVertexArray(sm->VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, sm->VBO[0]);
	glDrawArrays(GL_TRIANGLES, 0, sm->vertexcount);
	glEnd();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glutSwapBuffers();
	
}

vmath::mat4 SSREffect::inversion(vmath::mat4 m) 
{
	double src[4][4];
	double res[4][4];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			src[i][j] = m[i][j];
	GetMatrixInverse(src, 4, res);
	vmath::mat4 des;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			des[i][j] = res[i][j];
	return des;
}