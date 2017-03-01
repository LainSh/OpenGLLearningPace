#include <gl/glew.h>
#include <gl/freeglut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "SkinnedMesh.h"
#include "FBO.h"
#include "Common.h"

#ifndef WIN_WIDTH
#define WIN_WIDTH 1280
#endif // !WIN_WIDTH

#ifndef WIN_HEIGHT
#define WIN_HEIGHT 960
#endif // !WIN_HEIGHT

#ifndef PI
#define PI 3.141592653
#endif // !PI

CShader *pModelShader;
//CShader *pSceneShader;
CSkinnedMesh   *pMesh;

const int gLightsAmount = 1;
float gAngle = 0.0;
glm::vec3 gEyePos = glm::vec3(0.0, 20.0, 30.0);

float gStartTime;

void setLight()
{	
	GLuint PointLightNumLocation = pModelShader->getUniformLocation("uPointLightNum");
	GLuint DirectionalLightBaseAmbientIntensityLocation = pModelShader->getUniformLocation("uDirectionalLight.Base.AmbientIntensity");
	GLuint DirectionalLightBaseDiffuseIntensityLocation = pModelShader->getUniformLocation("uDirectionalLight.Base.DiffuseIntensity");
	GLuint DirectionalLightBaseColorLocation = pModelShader->getUniformLocation("uDirectionalLight.Base.Color");

	glm::vec3 DirectionalLightBaseColor = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 DirectionalLightDirection = glm::vec3(1.0, -1.0, 0.0);
	float DirectionalLightBaseAmbientIntensity = 0.6;
	float DirectionalLightBaseDiffuseIntensity = 0.8;

	glUniform1i(PointLightNumLocation, gLightsAmount);
	glUniform1f(DirectionalLightBaseAmbientIntensityLocation, DirectionalLightBaseAmbientIntensity);
	glUniform1f(DirectionalLightBaseDiffuseIntensityLocation, DirectionalLightBaseDiffuseIntensity);
	glUniform3f(DirectionalLightBaseColorLocation, DirectionalLightBaseColor.r, DirectionalLightBaseColor.g, DirectionalLightBaseColor.b);

	GLuint EyeWorldPosLocation = pModelShader->getUniformLocation("uEyeWorldPos");
	GLuint SpecularIntensityLocation = pModelShader->getUniformLocation("uSpecularIntensity");
	GLuint SpecularPowerLocation = pModelShader->getUniformLocation("uSpecularPower");

	float SpecularIntensity = 1.0;
	float SpecularPower = 32.0;
	glUniform3f(EyeWorldPosLocation, gEyePos.x, gEyePos.y, gEyePos.z);
	glUniform1f(SpecularPowerLocation, SpecularPower);
	glUniform1f(SpecularIntensityLocation, SpecularIntensity);

	SPointLightLocation PointLightLocation[gLightsAmount];
	for (unsigned int i=0; i<gLightsAmount; i++)
	{
		std::string UniformName;
		UniformName = "uPointLight[";
		char Number = '0' + i;
		UniformName.insert(UniformName.length(), 1, Number);
		UniformName.append("]");

		std::string UniformBaseColor = UniformName + ".Base.Color";
		PointLightLocation[i].m_BaseColorLocation = pModelShader->getUniformLocation(UniformBaseColor.c_str());
		std::string UniformBaseAmbientIntensity = UniformName + ".Base.AmbientIntensity";
		PointLightLocation[i].m_BaseAmbientIntensityLocation = pModelShader->getUniformLocation(UniformBaseAmbientIntensity.c_str());
		std::string UniformBaseDiffuseIntensity = UniformName + ".Base.DiffuseIntensity";
		PointLightLocation[i].m_BaseDiffuseIntensityLocation = pModelShader->getUniformLocation(UniformBaseDiffuseIntensity.c_str());
		std::string UniformPosition = UniformName + ".Position";
		PointLightLocation[i].m_PositionLocation = pModelShader->getUniformLocation(UniformPosition.c_str());
		std::string UniformAttenConstent = UniformName + ".Atten.Constant";
		PointLightLocation[i].m_AttenConstantLocation = pModelShader->getUniformLocation(UniformAttenConstent.c_str());
		std::string UniformAttenLinear = UniformName + ".Atten.Linear";
		PointLightLocation[i].m_AttenLinearLocation = pModelShader->getUniformLocation(UniformAttenLinear.c_str());
		std::string UniformAttenExp = UniformName + ".Atten.Exp";
		PointLightLocation[i].m_AttenExpLocation = pModelShader->getUniformLocation(UniformAttenExp.c_str());
	}

	SPointLight PointLight[2];
	PointLight[0].m_Color = glm::vec3(1.0, 0.5, 0.0);
	PointLight[0].m_Pos   = glm::vec3(0.0, 2.0, 0.0);
	PointLight[0].m_AmbientIntensity = 0.1;
	PointLight[0].m_DiffuseIntensity = 0.9;
	PointLight[0].m_AttenuationLinear = 0.2;
	PointLight[1].m_Color = glm::vec3(0.0, 0.5, 1.0);
	PointLight[1].m_Pos   = glm::vec3(4.0, 1.0, 0.0);
	PointLight[1].m_DiffuseIntensity = 0.5;
	PointLight[1].m_AttenuationLinear = 0.1;
	for (unsigned int i=0; i<gLightsAmount; i++)
	{
		glUniform3f(PointLightLocation[i].m_BaseColorLocation, PointLight[i].m_Color.r, PointLight[i].m_Color.g, PointLight[i].m_Color.b);
		glUniform3f(PointLightLocation[i].m_PositionLocation, PointLight[i].m_Pos.x, PointLight[i].m_Pos.y, PointLight[i].m_Pos.z);
		glUniform1f(PointLightLocation[i].m_BaseAmbientIntensityLocation, PointLight[i].m_AmbientIntensity);
		glUniform1f(PointLightLocation[i].m_BaseDiffuseIntensityLocation, PointLight[i].m_DiffuseIntensity);
		glUniform1f(PointLightLocation[i].m_AttenConstantLocation, PointLight[i].m_AttenuationConstent);
		glUniform1f(PointLightLocation[i].m_AttenLinearLocation, PointLight[i].m_AttenuationLinear);
		glUniform1f(PointLightLocation[i].m_AttenExpLocation, PointLight[i].m_AttenuationExp);
	}

}


void initializeUniformLocation()
{
	pModelShader->enable();

	for (unsigned int i=0; i<MAX_BONES; i++)
	{
		char Name[128];
		memset(Name, 0, sizeof(Name));
		_snprintf_s(Name, sizeof(Name), "uBones[%d]", i);

		gBoneLocation[i] = pModelShader->getUniformLocation(Name);
	}



	pModelShader->ban();
}

float getRunningTime()
{
	float RunningTime = (float)((double)glfwGetTime() - (double)gStartTime);
//	float RunningTime = (float)((double)GetTickCount() - (double)gStartTime) / 1000.0f;
	return RunningTime;
}

void setTransform()
{
	glm::mat4 TranslateMat = glm::translate(glm::mat4(1.0), glm::vec3(0.0,0.0,1.0));
	glm::mat4 RoatateMat   = glm::rotate(glm::mat4(1.0), gAngle, glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 ScaleMat     = glm::scale(glm::mat4(1.0), glm::vec3(0.1, 0.1, 0.1));
	glm::mat4 WorldTransformMat =  TranslateMat * RoatateMat * ScaleMat; 
	glm::mat4 ViewTransformMat = glm::lookAt(gEyePos, glm::vec3(0.0, 5.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	float aspect = WIN_WIDTH/ WIN_HEIGHT;
	glm::mat4 ProjectionTransformMat = glm::perspective(90.0f, aspect, 0.5f, 1000.0f);

	glm::mat4 WVPTransformMat = ProjectionTransformMat * ViewTransformMat * WorldTransformMat;

	glm::mat4 NormalMat = glm::transpose(glm::inverse(WorldTransformMat));

	GLuint NormalMatLocation = pModelShader->getUniformLocation("uNormalMat");
	glUniformMatrix4fv(NormalMatLocation, 1, GL_FALSE, glm::value_ptr(NormalMat));

	GLuint WVPTransformMatLocation = pModelShader->getUniformLocation("uWVPTransformMat");
	glUniformMatrix4fv(WVPTransformMatLocation, 1, GL_FALSE, glm::value_ptr(WVPTransformMat));

	GLuint WTransformMatLocation = pModelShader->getUniformLocation("uWTransformMat");
	glUniformMatrix4fv(WTransformMatLocation, 1, GL_FALSE, glm::value_ptr(WorldTransformMat));
}

void setTransformForMesh(glm::vec3 vTranslate)
{
//	glm::mat4 TranslateMat = glm::translate(glm::mat4(1.0), glm::vec3(10.0,0.0,1.0));
	glm::mat4 TranslateMat = glm::translate(glm::mat4(1.0), vTranslate);
	glm::mat4 RoatateMat   = glm::rotate(glm::mat4(1.0), gAngle, glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 ScaleMat     = glm::scale(glm::mat4(1.0), glm::vec3(0.1, 0.1, 0.1));
	glm::mat4 WorldTransformMat =  TranslateMat * RoatateMat * ScaleMat; 
	glm::mat4 ViewTransformMat = glm::lookAt(gEyePos, glm::vec3(0.0, 5.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	float aspect = WIN_WIDTH/ WIN_HEIGHT;
	glm::mat4 ProjectionTransformMat = glm::perspective(90.0f, aspect, 0.5f, 1000.0f);

	glm::mat4 WVPTransformMat = ProjectionTransformMat * ViewTransformMat * WorldTransformMat;

	glm::mat4 NormalMat = glm::transpose(glm::inverse(WorldTransformMat));

	GLuint NormalMatLocation = pModelShader->getUniformLocation("uNormalMat");
	glUniformMatrix4fv(NormalMatLocation, 1, GL_FALSE, glm::value_ptr(NormalMat));

	GLuint WVPTransformMatLocation = pModelShader->getUniformLocation("uWVPTransformMat");
	glUniformMatrix4fv(WVPTransformMatLocation, 1, GL_FALSE, glm::value_ptr(WVPTransformMat));

	GLuint WTransformMatLocation = pModelShader->getUniformLocation("uWTransformMat");
	glUniformMatrix4fv(WTransformMatLocation, 1, GL_FALSE, glm::value_ptr(WorldTransformMat));
}


void setBoneTransform(unsigned int vIndex, const glm::mat4& vMatrix)
{
	_ASSERT(vIndex < MAX_BONES);
	glUniformMatrix4fv(gBoneLocation[vIndex], 1, GL_FALSE, glm::value_ptr(vMatrix));
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	pModelShader->enable();

	float RunningTime = glfwGetTime();
	std::vector<glm::mat4> BoneTransform;
//	RunningTime = 0.202999994;
	pMesh->boneTransform(RunningTime, BoneTransform);

	for (unsigned int i=0; i<BoneTransform.size(); i++)
	{
		setBoneTransform(i, BoneTransform[i]);
	}
	setTransform();
	setLight();

	pMesh->render();

	glDisable(GL_DEPTH_TEST);
	pModelShader->ban();
}

bool initializeModel(const std::string& vModelFileName)
{
	pMesh = new CSkinnedMesh;
	return pMesh->loadMesh(vModelFileName.c_str());
}

void initializeShader()
{
	pModelShader = new CShader;
	pModelShader->addShader(GL_VERTEX_SHADER, "shader/skinningV.glsl");
	pModelShader->addShader(GL_FRAGMENT_SHADER, "shader/skinningF.glsl");
	pModelShader->compileShader();

}

void initializeGlutCallback()
{
	glutDisplayFunc(renderScene);
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "SkeletalAnimation", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Init GLEW here*/
	GLuint res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return -1;
	}

	initializeShader();
	initializeModel("nudeman02_run.FBX");

//	gStartTime = glfwGetTime();
	initializeUniformLocation();
	glfwSetTime(0);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		renderScene();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);
		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}