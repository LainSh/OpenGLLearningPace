#include "Mesh.h"
#include "Texture.h"

CMesh::CMesh(void)
{
}


CMesh::~CMesh(void)
{
	__clear();
}

//******************************************************************
//FUNCTION:
CMesh::SMeshEntry::SMeshEntry()
{
	VB = INVALID_VALUE;
	IB = INVALID_VALUE;
	NumIndices  = 0;
	MaterialIndex = INVALID_MATERIAL;
}

//******************************************************************
//FUNCTION:
CMesh::SMeshEntry::~SMeshEntry()
{
	if (VB != INVALID_VALUE)
	{
		glDeleteBuffers(1, &VB);
	}

	if (IB != INVALID_VALUE)
	{
		glDeleteBuffers(1, &IB);
	}
}

//******************************************************************
//FUNCTION:
void CMesh::SMeshEntry::Init(const std::vector<SVertex>& Vertices, const std::vector<unsigned int>& Indices)
{
	NumIndices = Indices.size();

	glGenBuffers(1, &VB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SVertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &IB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * NumIndices, &Indices[0], GL_STATIC_DRAW);
}

//******************************************************************
//FUNCTION:
bool CMesh::loadMesh(const std::string& vFileName)
{
	__clear();

	bool Ret = false;
	Assimp::Importer Importer;

	const aiScene *pScene = Importer.ReadFile(vFileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (pScene)
	{
		Ret = __initFromScene(pScene, vFileName);
	}
	else {
		printf("Error parsing '%s': '%s'\n", vFileName.c_str(), Importer.GetErrorString());
	}

	return Ret;
}

//******************************************************************
//FUNCTION:
void CMesh::render()
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
		glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VB);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (const GLvoid*)12);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (const GLvoid*)20);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].IB);

		const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

		if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
			m_Textures[MaterialIndex]->bind(GL_TEXTURE0);
		}

		glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

//******************************************************************
//FUNCTION:
void CMesh::__clear()
{
	for (unsigned int i = 0 ; i < m_Textures.size() ; i++) {
		SAFE_DELETE(m_Textures[i]);
	}
}

//******************************************************************
//FUNCTION:
bool CMesh::__initFromScene(const aiScene* vScene, const std::string& vFileName)
{
	m_Entries.resize(vScene->mNumMeshes);
	m_Textures.resize(vScene->mNumMaterials);

	for (unsigned int i=0; i<m_Entries.size(); i++)
	{
		const aiMesh *pAiMesh = vScene->mMeshes[i];
		__initMesh(i, pAiMesh);
	}

	return __initMaterials(vScene, vFileName);
//	return true;
}

//******************************************************************
//FUNCTION:
void CMesh::__initMesh(unsigned int vIndex, const aiMesh* vAimesh)
{
	m_Entries[vIndex].MaterialIndex = vAimesh->mMaterialIndex;

	std::vector<SVertex> Vertices;
	std::vector<unsigned int> Indices;

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	for (unsigned int i=0; i<vAimesh->mNumVertices; i++)
	{
		const aiVector3D* pPos      = &(vAimesh->mVertices[i]);
		const aiVector3D* pNormal   = &(vAimesh->mNormals[i]);
		const aiVector3D* pTexCoord = vAimesh->HasTextureCoords(0) ? &(vAimesh->mTextureCoords[0][i]) : &Zero3D;

		SVertex Vertex(glm::vec3(pPos->x, pPos->y, pPos->z),
					   glm::vec2(pTexCoord->x, pTexCoord->y),
					   glm::vec3(pNormal->x, pNormal->y, pNormal->z));

		Vertices.push_back(Vertex);
	}

	for (unsigned int i=0; i<vAimesh->mNumFaces; i++)
	{
		const aiFace& Face = vAimesh->mFaces[i];
		_ASSERT(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}
	m_Entries[vIndex].Init(Vertices, Indices);
}

//******************************************************************
//FUNCTION:
bool CMesh::__initMaterials(const aiScene* vScnen, const std::string& vFileName)
{
	std::string::size_type SlashIndex = vFileName.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos)
	{
		Dir = ".";
	}
	else if (SlashIndex == 0)
	{
		Dir = "/";
	}
	else
	{
		Dir = vFileName.substr(0, SlashIndex);
	}
	bool Ret = true;

	for (unsigned int i=0; i<vScnen->mNumMaterials; i++)
	{
		const aiMaterial* pMaterial = vScnen->mMaterials[i];

		m_Textures[i] = NULL;
		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE)>0)
		{
			aiString Path;
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				std::string FullPath = Path.data;
				m_Textures[i]  = new CTexture(GL_TEXTURE_2D, FullPath.c_str());

				if (!m_Textures[i]->load())
				{
					printf("Error loading texture '%s'\n", FullPath.c_str());
					delete m_Textures[i];
					m_Textures[i] = NULL;
					Ret = false;
				}
				else
				{
					printf("Loaded texture '%s'\n", FullPath.c_str());
				}
			}
			if (!m_Textures[i]) {
				m_Textures[i] = new CTexture(GL_TEXTURE_2D, "white.png");
				Ret = m_Textures[i]->load();
			}
		}
	}

	return Ret;
}