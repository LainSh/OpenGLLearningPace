#pragma once
#include<iostream>
#include<gl/glew.h>
#pragma comment(lib, "glew32.lib")

const int   TEXTURE_WIDTH   = 1280; 
const int   TEXTURE_HEIGHT  = 720; 

class CFBO
{
public:
	CFBO(){}
	~CFBO(){}
	void creatFBO();
	void bindFBO();
	void banFBO();
	void addBufferToFBO(GLuint &bufferId, unsigned int n);
private:
	bool checkFramebufferStatus();
	GLuint m_fboId;
	GLuint m_depId;
};

