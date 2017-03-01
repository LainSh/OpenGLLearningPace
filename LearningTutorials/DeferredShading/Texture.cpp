#include "Texture.h"
#include <SOIL/SOIL.h>

CTexture::CTexture(GLenum vTextureTarget, const std::string& vFileName)
{
	m_FileName = vFileName;
	m_TextureTarget = vTextureTarget;
	m_Image = NULL;
}


CTexture::~CTexture(void)
{
	SOIL_free_image_data(m_Image);
}

//******************************************************************
//FUNCTION:
bool CTexture::load()
{
	m_Image = SOIL_load_image(m_FileName.c_str(), &m_Width, &m_Height, &m_Channel, SOIL_LOAD_RGB);

	if (!m_Image)
	{
		return false;
	}

	glGenTextures(1, &m_TextureObj);
	glBindTexture(m_TextureTarget, m_TextureObj);
	glTexParameterf(m_TextureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(m_TextureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(m_TextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(m_TextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(m_TextureTarget, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(m_TextureTarget, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_Image);

	glBindTexture(m_TextureTarget, 0);

	return true;
}

//******************************************************************
//FUNCTION:
void CTexture::bind(GLenum vTextureUnit)
{
	glActiveTexture(vTextureUnit);
	glBindTexture(m_TextureTarget, m_TextureObj);
}