#pragma once
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>

class CGeometry
{
public:
	enum EGeometryType
	{
		GEO_UNKNOWN_TYPE = 0,
		GEO_POINT,
		GEO_LINE,
		GEO_TRIANGLE,
		GEO_TRIANGLE_FAN,
		GEO_QUAD
	};

public:
	CGeometry(void) {}
	virtual ~CGeometry(void) {}

	void render();

protected:
	void _initGeometryObject();
	void _attachVertices2GeomtryObject();
	void _setType(EGeometryType vType) { m_Type = vType;}

	void _clearVertices() { m_Vertices.clear(); m_VerticesNum = 0; }
	void _addVertex(const glm::vec3& vVertex) { m_Vertices.push_back(vVertex); m_VerticesNum++; }

	virtual void _render(GLenum vDrawType);

private:
	GLuint m_VBO;
	unsigned int m_VerticesNum;
	std::vector<glm::vec3> m_Vertices;
	EGeometryType m_Type;
	std::map<EGeometryType, GLenum> m_DrawTypeMap;

	void __initDrawMap();
};

/************************************************************************/
/* QUAD CLASS                                                           */
/************************************************************************/
class CQuad : public CGeometry
{
public:
	CQuad(void);
	virtual ~CQuad(void) {}

	void setFullScreenQuad(int vScreenWidth, int vScreenHeight);
};