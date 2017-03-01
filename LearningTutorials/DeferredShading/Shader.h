#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <string>
#include <list>

class CShader
{
public:
	CShader(void);
	~CShader(void);

	bool addShader(GLenum vShaderType, const std::string& vShaderFileName);
	bool compileShader();
	GLuint getUniformLocation(const std::string& vUniformName);
	void enable();
	void disable();

	bool setUniformInt(const std::string& vUniformName, int vValue);
	bool setUniformFloat(const std::string& vUniformName, float vValue);
	bool setUniformVec2(const std::string& vUniformName, float vValueX, float vValueY);
	bool setUniformVec3(const std::string& vUniformName, float vValueX, float vValueY, float vValueZ);
	bool setUniformVec4(const std::string& vUniformName, float vValueX, float vValueY, float vValueZ, float vValueW);

	bool isUniformValidated() {return m_IsUniformValidated;}

private:
	GLuint m_ShaderProgram;
	std::list<GLuint> m_ShaderObjectList;
	bool m_IsUniformValidated;

	bool __readShaderFile(const std::string& vShaderFileName, std::string& voShaderData);
};

