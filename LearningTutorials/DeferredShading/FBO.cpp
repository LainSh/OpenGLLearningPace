#include "FBO.h"

void CFBO::bindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
}

void CFBO::banFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CFBO::creatFBO(unsigned int vTexWidth, unsigned int vTexHeight)
{ 
	m_TextureWidth = vTexWidth;
	m_TexTureHeight = vTexHeight;

	glGenFramebuffers(1, &m_fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
//	glGenRenderbuffers(1, &m_depId); 
	glGenTextures(1, &m_depId);
	glBindTexture(GL_TEXTURE_2D, m_depId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, vTexWidth, vTexHeight, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depId, 0);
// 	glBindRenderbuffer(GL_RENDERBUFFER, m_depId);  
// 	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_TextureWidth, m_TexTureHeight);  
// 	glBindRenderbuffer(GL_RENDERBUFFER, 0); 
// 	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depId);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);  
}

void CFBO::addBufferToFBO(GLuint &bufferId, unsigned int n)
{
	bool status;
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);

	glGenTextures(1, &bufferId);  
	glBindTexture(GL_TEXTURE_2D, bufferId);  
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_TextureWidth, m_TexTureHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0); 

	switch(n)
	{
	case 0:
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferId, 0);
		break;
	case 1:
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferId, 0);
		break;
	case 2:
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, bufferId, 0); 
		break;
	case 3:
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, bufferId, 0); 
		break;
	case 4:
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, bufferId, 0); 
		break;
	case 5:
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, bufferId, 0); 
		break;
	case 6:
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, bufferId, 0);
		break;
	case 7:
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, GL_TEXTURE_2D, bufferId, 0);
		break;
	case 8:
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT8, GL_TEXTURE_2D, bufferId, 0);
		break;
	case 9:
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT9, GL_TEXTURE_2D, bufferId, 0);
	default :
		std::cout<<"Ìí¼Ó»º´æµ½FBOÊ§°Ü"<<std::endl;
	}
	status = checkFramebufferStatus();
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}

bool CFBO::checkFramebufferStatus()
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch(status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		std::cout << "Framebuffer complete." << std::endl;
		return true;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cout << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
		return false;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cout << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
		return false;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cout << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
		return false;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cout << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
		return false;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cout << "[ERROR] Framebuffer incomplete: Unsupported by FBO implementation." << std::endl;
		return false;

	default:
		std::cout << "[ERROR] Framebuffer incomplete: Unknown error." << std::endl;
		return false;
	}
}