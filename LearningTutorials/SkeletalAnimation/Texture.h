#pragma once
#include <gl/glew.h>
#include <string>

class CTexture
{
public:
	CTexture(GLenum vTextureTarget, const std::string& vFileName);
	~CTexture(void);

	bool load();
	void bind(GLenum vTextureUnit);

private:
	std::string m_FileName;
	GLenum m_TextureTarget;
	GLuint m_TextureObj;
	unsigned char* m_Image;
	int m_Width;
	int m_Height;
	int m_Channel;
};

