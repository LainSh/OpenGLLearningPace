#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/matrix4x4.h>

glm::mat4 mat4FromaiMatrix4x4(const aiMatrix4x4& vAssimpMat4)
{
	glm::mat4 retMatrix;

	float Value[16];

	Value[0] = vAssimpMat4.a1; Value[1] = vAssimpMat4.b1; Value[2] = vAssimpMat4.c1; Value[3] = vAssimpMat4.d1;
	Value[4] = vAssimpMat4.a2; Value[5] = vAssimpMat4.b2; Value[6] = vAssimpMat4.c2; Value[7] = vAssimpMat4.d2;
	Value[8] = vAssimpMat4.a3; Value[9] = vAssimpMat4.b3; Value[10]= vAssimpMat4.c3; Value[11]= vAssimpMat4.d3;
	Value[12]= vAssimpMat4.a4; Value[13]= vAssimpMat4.b4; Value[14]= vAssimpMat4.c4; Value[15]= vAssimpMat4.d4;

	retMatrix = glm::make_mat4(Value);
	return retMatrix;
};

glm::mat4 mat4FromaiMatrix3x3(const aiMatrix3x3& vAssimpMat3)
{
	glm::mat4 retMatrix;

	float Value[16];

	Value[0] = vAssimpMat3.a1; Value[1] = vAssimpMat3.b1; Value[2] = vAssimpMat3.c1; Value[3] = 0.0;
	Value[4] = vAssimpMat3.a2; Value[5] = vAssimpMat3.b2; Value[6] = vAssimpMat3.c2; Value[7] = 0.0;
	Value[8] = vAssimpMat3.a3; Value[9] = vAssimpMat3.b3; Value[10]= vAssimpMat3.c3; Value[11]= 0.0;
	Value[12]= 0.0;			   Value[13]= 0.0;			  Value[14]= 0.0;			 Value[15]= 1.0;

	retMatrix = glm::make_mat4(Value);
	return retMatrix;
};