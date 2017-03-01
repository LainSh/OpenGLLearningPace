#pragma once
#include<iostream>
#include<gl/glew.h>
#pragma comment(lib, "glew32.lib")

class CFBO
{
public:
	CFBO(){}
	~CFBO(){}
	void creatFBO(unsigned int vTexWidth, unsigned int vTexHeight);
	void bindFBO();
	void banFBO();
	void addBufferToFBO(GLuint &bufferId, unsigned int n);
private:
	bool checkFramebufferStatus();
	GLuint m_fboId;
	GLuint m_depId;
	GLuint m_stencilId;

	unsigned int m_TextureWidth;
	unsigned int m_TexTureHeight;
};

