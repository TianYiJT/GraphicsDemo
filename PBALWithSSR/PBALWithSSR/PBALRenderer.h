#pragma once
#include"glConfigruation.h"
class PBALRenderer 
{
public:
	void Render(SceneManager *sm);
	void Init();
private:
	void preCompute();
	vmath::vec4 *specularLookUpTex;
	vmath::vec4 *diffLookUpTex;
	GLuint PBAL_Shader;
	GLuint DiffLookUpTex[1];
	GLuint SpecularLookUpTex[1];
	int xl=100, yl=100, zl=50;
	float cd=0.6;
	float ln=0.9;
	float Intensity=0.8;
	float rMax=10;
	float glossness=1;
	float frenel=0.64;
	float Roughness=1;
	vmath::vec3 viewpoint;
	GLuint viewpoint_loc;
	GLuint mvp_loc;
	GLuint lightnumber_loc;
	GLuint DiffuseLookUpTex_loc;
	GLuint SpecularLookUpTex_loc;
	GLuint TexLookUpTex_loc;
	GLuint cd_loc;
	GLuint Intensity_loc;
	GLuint ln_loc;
	GLuint rMax_loc;
	GLuint glossness_loc;
	GLuint frenel_loc;

	GLuint light1_loc;
	GLuint light2_loc;
	GLuint light3_loc;
	GLuint light4_loc;
	GLuint light5_loc;
	GLuint light6_loc;
	GLuint light7_loc;

	GLuint lightType1_loc;
	GLuint lightType2_loc;
	GLuint lightType3_loc;
	GLuint lightType4_loc;
	GLuint lightType5_loc;
	GLuint lightType6_loc;
	GLuint lightType7_loc;
};
float BRDF_D(vmath::vec3 h, float g,vmath::vec3 Nor)
{
	float NdotH = vmath::dot(vmath::normalize(h), vmath::normalize(Nor));
	float alpha2 = g*g;
	return alpha2 / (3.1415926*(pow((NdotH*NdotH*(alpha2 - 1) + 1), 2)));
}
float BRDF_F(vmath::vec3 w, vmath::vec3 h,float frensel)
{
	float WDotH = dot(normalize(w), normalize(h));
	const float f1 = -5.55473f;
	float f_ = (f1*WDotH - 6.98316f)*WDotH;
	return frensel + (1 - frensel)*pow(2, f_);
}
float BRDF_G1(vmath::vec3 v,float Roughness,vmath::vec3 Nor)
{
	float k = (Roughness + 1)*(Roughness + 1) / 8;
	float VDotN = dot(normalize(v), normalize(Nor));
	return VDotN / (VDotN*(1 - k) + k);
}
float BRDF_G(vmath::vec3 w1, vmath::vec3 w2, vmath::vec3 h,float Roughness,vmath::vec3 Nor)
{
	return BRDF_G1(w1,Roughness,Nor)*BRDF_G1(w2,Roughness,Nor);
}
void PBALRenderer::Init()
{
	specularLookUpTex = new vmath::vec4[xl*yl*zl];
	diffLookUpTex = new vmath::vec4[xl*yl*zl];
	preCompute();
	glGenTextures(1, DiffLookUpTex);
	glBindTexture(GL_TEXTURE_3D,DiffLookUpTex[0]);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, xl, yl, zl, 0, GL_RGBA, GL_FLOAT, diffLookUpTex);
	glGenTextures(1, SpecularLookUpTex);	
	glBindTexture(GL_TEXTURE_3D, SpecularLookUpTex[0]);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, xl, yl, zl, 0, GL_RGBA, GL_FLOAT, specularLookUpTex);


	ShaderInfo si[] = { {GL_VERTEX_SHADER,PBAL_VERT_SHADER},{GL_FRAGMENT_SHADER,PBAL_FRAG_SHADER},{GL_NONE,NULL} };
	PBAL_Shader = LoadShaders(si);
	viewpoint_loc = glGetUniformLocation(PBAL_Shader, "viewpoint");
	mvp_loc = glGetUniformLocation(PBAL_Shader,"mvp");
	lightnumber_loc = glGetUniformLocation(PBAL_Shader,"lightnumber");
	DiffuseLookUpTex_loc = glGetUniformLocation(PBAL_Shader,"DiffuseLookUpTex");
	SpecularLookUpTex_loc = glGetUniformLocation(PBAL_Shader, "SpecularLookUpTex");
	TexLookUpTex_loc = glGetUniformLocation(PBAL_Shader, "TexLookUpTex");
	cd_loc = glGetUniformLocation(PBAL_Shader, "cd");
	Intensity_loc = glGetUniformLocation(PBAL_Shader, "Intensity");
	ln_loc = glGetUniformLocation(PBAL_Shader, "ln");
	rMax_loc = glGetUniformLocation(PBAL_Shader, "rMax");
	glossness_loc = glGetUniformLocation(PBAL_Shader, "glossness");
	frenel_loc = glGetUniformLocation(PBAL_Shader, "frenel");

	light1_loc = glGetUniformLocation(PBAL_Shader, "light1");
	light2_loc = glGetUniformLocation(PBAL_Shader, "light2");
	light3_loc = glGetUniformLocation(PBAL_Shader, "light3");
	light4_loc = glGetUniformLocation(PBAL_Shader, "light4");
	light5_loc = glGetUniformLocation(PBAL_Shader, "light5");
	light6_loc = glGetUniformLocation(PBAL_Shader, "light6");
	light7_loc = glGetUniformLocation(PBAL_Shader, "light7");

	lightType1_loc = glGetUniformLocation(PBAL_Shader, "lightType1");
	lightType2_loc = glGetUniformLocation(PBAL_Shader, "lightType2");
	lightType3_loc = glGetUniformLocation(PBAL_Shader, "lightType3");
	lightType4_loc = glGetUniformLocation(PBAL_Shader, "lightType4");
	lightType5_loc = glGetUniformLocation(PBAL_Shader, "lightType5");
	lightType6_loc = glGetUniformLocation(PBAL_Shader, "lightType6");
	lightType7_loc = glGetUniformLocation(PBAL_Shader, "lightType7");
}
void PBALRenderer::Render(SceneManager *sm)
{
	glUseProgram(PBAL_Shader);
	glUniform3f(viewpoint_loc,viewpoint[0],viewpoint[1],viewpoint[2]);
	glUniformMatrix4fv(mvp_loc, 1, GL_TRUE, sm->mvp.buffer());
	glUniform1i(lightnumber_loc, sm->lp_count);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, DiffLookUpTex[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, SpecularLookUpTex[0]);
	glUniform1i(DiffuseLookUpTex_loc,0);
	glUniform1i(SpecularLookUpTex_loc, 1);
	glUniform1f(cd_loc,cd);
	glUniform1f(Intensity_loc,Intensity);
	glUniform1f(ln_loc,ln);
	glUniform1f(rMax_loc,rMax);
	glUniform1f(glossness_loc,glossness);
	glUniform1f(frenel_loc,frenel);
	glUniformMatrix4fv(light1_loc,1,GL_FALSE,sm->lp_mat[0]);
	glUniformMatrix4fv(light2_loc, 1, GL_FALSE, sm->lp_mat[1]);
	glUniformMatrix4fv(light3_loc, 1, GL_FALSE, sm->lp_mat[2]);
	glUniformMatrix4fv(light4_loc, 1, GL_FALSE, sm->lp_mat[3]);
	glUniformMatrix4fv(light5_loc, 1, GL_FALSE, sm->lp_mat[4]);
	glUniformMatrix4fv(light6_loc, 1, GL_FALSE, sm->lp_mat[5]);
	glUniformMatrix4fv(light7_loc, 1, GL_FALSE, sm->lp_mat[6]);

	glUniform1i(lightType1_loc,sm->lp[0].type);
	glUniform1i(lightType2_loc, sm->lp[1].type);
	glUniform1i(lightType3_loc, sm->lp[2].type);
	glUniform1i(lightType4_loc, sm->lp[3].type);
	glUniform1i(lightType5_loc, sm->lp[4].type);
	glUniform1i(lightType6_loc, sm->lp[5].type);
	glUniform1i(lightType7_loc, sm->lp[6].type);
	vmath::vec3 * pos = sm->pos;
	vmath::vec3 * nor = sm->nor;
	vmath::vec3 * col = sm->col;
	float * roughness = sm->roughness;
	glBufferData(GL_ARRAY_BUFFER, sizeof(vmath::vec3) * 3 * sm->vertexcount+sizeof(float)*sm->vertexcount ,NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vmath::vec3) * sm->vertexcount, pos[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vmath::vec3) * sm->vertexcount, sizeof(vmath::vec3) * sm->vertexcount, nor[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vmath::vec3) * sm->vertexcount * 2, sizeof(vmath::vec3) * sm->vertexcount, col[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vmath::vec3)*sm->vertexcount * 3, sizeof(float)*sm->vertexcount, roughness);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vmath::vec3), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vmath::vec3), BUFFER_OFFSET(sizeof(vmath::vec3) * sm->vertexcount));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vmath::vec3), BUFFER_OFFSET(sizeof(vmath::vec3) * sm->vertexcount*2));
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vmath::vec3) * sm->vertexcount * 3));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glBindVertexArray(sm->VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, sm->VBO[0]);
	glDrawArrays(GL_TRIANGLES, 0, sm->vertexcount);
	glEnd();
}

void PBALRenderer::preCompute() 
{
	for (int i = 0; i < xl; i++) 
	{
		for (int j = 0; j < yl; j++) 
		{
			for (int k = 0; k < zl; k++) 
			{
				float tempX = (float)i / (float)xl;
				float tempY = (float)j / (float)yl;
				float tempZ = (float)k / (float)zl;
				float tempK = pow(tempX*tempX + tempY*tempY+tempZ*tempZ, 0.5f);
				diffLookUpTex[i*yl*zl+j*zl+k] = vmath::vec4(tempZ / tempK,0,0,0);
				vmath::vec3 w0 = viewpoint - vmath::vec3(tempX,tempY,0);
				vmath::vec3 w1 = vmath::vec3(tempX,tempY,0);
				vmath::vec3 h = (w0 + w1) / 2;
				vmath::vec3 n = (0, 0, 1);
				specularLookUpTex[i*yl*zl+j*zl+k] = vmath::vec4(BRDF_D(h, tempZ, n)*BRDF_F(w0, h, frenel)*BRDF_G(w0, w1, h, Roughness, n)/(4*vmath::dot(n,w1)),0,0,0);
			}
		}
	}
}