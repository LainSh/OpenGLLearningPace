#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <vector>

class CTexture;

class CMesh
{
protected:
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }
#define INVALID_MATERIAL 0xFFFFFFFF
#define INVALID_VALUE	 0xffffffff

	struct SVertex
	{
		glm::vec3 m_pos;
		glm::vec2 m_tex;
		glm::vec3 m_normal;

		SVertex() {}

		SVertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& normal)
		{
			m_pos    = pos;
			m_tex    = tex;
			m_normal = normal;
		}
	};

	struct SMeshEntry
	{
		SMeshEntry();

		~SMeshEntry();

		void Init(const std::vector<SVertex>& Vertices,
			const std::vector<unsigned int>& Indices);

		GLuint VB;
		GLuint IB;
		unsigned int NumIndices;
		unsigned int MaterialIndex;
	};

public:
	CMesh(void);
	~CMesh(void);

	bool loadMesh(const std::string& vFileName);
	void render();

private:
	std::vector<SMeshEntry> m_Entries;
	std::vector<CTexture*> m_Textures;

	bool __initFromScene(const aiScene* vScene, const std::string& vFileName);
	void __initMesh(unsigned int vIndex, const aiMesh* vAimesh);
	bool __initMaterials(const aiScene* vScnen, const std::string& vFileName);
	void __clear();
};

