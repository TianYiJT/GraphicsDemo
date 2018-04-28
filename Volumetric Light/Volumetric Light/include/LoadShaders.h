#pragma once
#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include<glew.h> 
#include"freeglut.h"
struct ShaderInfo
{
	GLuint Shader_Code;
	char * path;
};
GLuint LoadShader(const char * context, GLuint _type)
{
	GLuint shader_code = glCreateShader(_type);
	glShaderSource(shader_code, 1, &context, NULL);
	glCompileShader(shader_code);
	
	return shader_code;
}
char * Load_A_Shader_File(char * _path)
{
	FILE *fp;
	char * context = NULL;
	int count = 0;
	if (_path != NULL)
	{
		fp = fopen(_path, "rt");
		if (fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);
			if (count>0)
			{
				context = (char*)malloc(sizeof(char)*(count + 1));
				count = fread(context, sizeof(char), count, fp);
				context[count] = '\0';
			}
			fclose(fp);
		}
	}
	return context;
}
GLuint LoadShaders(ShaderInfo * _SI) 
{
	GLuint _p = glCreateProgram();
	while (_SI->Shader_Code!=GL_NONE)
	{
		const char * context = Load_A_Shader_File(_SI->path);
		GLuint _shadercode = LoadShader(context, _SI->Shader_Code);
		glAttachShader(_p,_shadercode);
		_SI++;
	}
	glLinkProgram(_p);
	return _p;
}

