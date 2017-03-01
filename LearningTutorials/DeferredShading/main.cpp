#include <gl/glew.h>
#include <gl/freeglut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Mesh.h"
#include "FBO.h"
#include "Light.h"
#include "Geometry.h"

#define WIN_WIDTH 1024
#define WIN_HEIGHT 768

#define LightDistance 10

glm::vec3 gModelPosition(0.0, 0.0, 0.0);
glm::vec3 gLightPosition(0.0, 3.0, 1.0);
glm::vec3 gEyePos(0.0, 0.0, 15.0);
glm::vec3 gTargetPos(0.0, 0.0, -10.0);

CShader *pGeomShader;
CShader *pStencilShader;
CShader *pPointLightShader;
CShader *pDirectionalLightShader;
CShader *pOutputTextureShader;
CShader *pCompositeShader;

GLuint DepthTex;
GLuint WorldTex;
GLuint DiffuseTex;
GLuint NormalTex;
GLuint TexcoordTex;

GLuint PointLightShadingTex;
GLuint DirectionalLightShadingTex;
GLuint CompositeShadingTex;

CMesh *pBaseSphere;
CMesh *pModel;
CFBO *pGBuffer;

CLight *pPointLight;
CLight *pDirectionalLight;
CQuad  *pScreenQuad;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_W :
			gModelPosition.y += 0.5;
			break;
		case GLFW_KEY_S :
			gModelPosition.y -= 0.5;
			break;
		case GLFW_KEY_A :
			gModelPosition.x -= 0.5;
			break;
		case GLFW_KEY_D :
			gModelPosition.x += 0.5;
			break;
		case GLFW_KEY_Q :
			gModelPosition.z -= 0.5;
			break;
		case GLFW_KEY_E :
			gModelPosition.z += 0.5;
			break;
		case GLFW_KEY_LEFT : 
			gLightPosition.x -= 0.5;
			break;
		case GLFW_KEY_RIGHT:
			gLightPosition.x += 0.5;
			break;
		case GLFW_KEY_UP:
			gLightPosition.y += 0.5;
			break;
		case GLFW_KEY_DOWN:
			gLightPosition.y -= 0.5;
			break;
		case GLFW_KEY_PAGE_UP:
			gLightPosition.z += 0.5;
			break;
		case GLFW_KEY_PAGE_DOWN:
			gLightPosition.z -= 0.5;
		default:
			break;
		}
	}
}

void initLight()
{
	pPointLight = new CLight(CLight::POINT_LIGHT);
	pPointLight->setLightColor(0.7, 0.0, 0.0);
	pPointLight->setAmbientIntensity(0.0);
	pPointLight->setDiffuseIntensity(0.8);
	pPointLight->setLightPositionOrDirection(gLightPosition);
	pPointLight->setLightAttenuation(0.0, 0.0, 0.0);

	pDirectionalLight = new CLight(CLight::DIRECTIONAL_LIGHT);
	pDirectionalLight->setLightColor(1.0, 1.0, 1.0);
	pDirectionalLight->setAmbientIntensity(0.1);
	pDirectionalLight->setDiffuseIntensity(0.8);
	pDirectionalLight->setLightPositionOrDirection(1.0, -1.0, 0.0);
}

void initGeometryUniform()
{
	glm::mat4 TranslateMat = glm::translate(glm::mat4(1.0), gModelPosition);
	glm::mat4 RotateMat    = glm::mat4(1.0);
	glm::mat4 WorldTransformMat = TranslateMat * RotateMat;

	glm::mat4 ViewTransformMat = glm::lookAt(gEyePos, gTargetPos, glm::vec3(0.0, 1.0, 0.0));

	glm::mat4 ProjectionTransformMat = glm::perspective(glm::radians(90.0f), (float)WIN_WIDTH/(float)WIN_HEIGHT, 0.1f, 1000.f);

	glm::mat4 WVPTransformMat = ProjectionTransformMat * ViewTransformMat * WorldTransformMat;

	glm::mat4 NormalMat = glm::transpose(glm::inverse(WorldTransformMat));

	GLuint WVPTransformLocation = pGeomShader->getUniformLocation("uWVPTransformMat");
	GLuint WTransformLocation = pGeomShader->getUniformLocation("uWTransformMat");
	GLuint NormalMatLocation = pGeomShader->getUniformLocation("uNormalMat");
	glUniformMatrix4fv(WVPTransformLocation, 1, GL_FALSE, glm::value_ptr(WVPTransformMat));
	glUniformMatrix4fv(WTransformLocation, 1, GL_FALSE, glm::value_ptr(WorldTransformMat));
	glUniformMatrix4fv(NormalMatLocation, 1, GL_FALSE, glm::value_ptr(NormalMat));
	
	pGeomShader->setUniformInt("uColorSampler", 0);
}

void initStentcilUniform()
{
	glm::mat4 TranslateMat = glm::translate(glm::mat4(1.0), pPointLight->getLightPositionOrDirection());
	glm::mat4 RotateMat    = glm::mat4(1.0);
	glm::mat4 ScaleMat     = glm::scale(glm::mat4(1.0), glm::vec3(LightDistance, LightDistance, LightDistance));
	glm::mat4 WorldTransformMat = TranslateMat * RotateMat * ScaleMat;
	glm::mat4 ViewTransformMat = glm::lookAt(gEyePos, gTargetPos, glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 ProjectionTransformMat = glm::perspective(glm::radians(90.0f), (float)WIN_WIDTH/(float)WIN_HEIGHT, 0.1f, 1000.f);
	glm::mat4 WVPTransformMat = ProjectionTransformMat * ViewTransformMat * WorldTransformMat;
	GLuint WVPTransformLocation = pStencilShader->getUniformLocation("uWVPTransformMat");
	glUniformMatrix4fv(WVPTransformLocation, 1, GL_FALSE, glm::value_ptr(WVPTransformMat));
}

void initPointLightUniform()
{
	glm::mat4 TranslateMat = glm::translate(glm::mat4(1.0), pPointLight->getLightPositionOrDirection());
	glm::mat4 RotateMat    = glm::mat4(1.0);
	glm::mat4 ScaleMat     = glm::scale(glm::mat4(1.0), glm::vec3(LightDistance, LightDistance, LightDistance));
	glm::mat4 WorldTransformMat = TranslateMat * RotateMat * ScaleMat;
	glm::mat4 ViewTransformMat = glm::lookAt(gEyePos, gTargetPos, glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 ProjectionTransformMat = glm::perspective(glm::radians(90.0f), (float)WIN_WIDTH/(float)WIN_HEIGHT, 0.1f, 1000.f);
	glm::mat4 WVPTransformMat = ProjectionTransformMat * ViewTransformMat * WorldTransformMat;
	GLuint WVPTransformLocation = pPointLightShader->getUniformLocation("uWVPTransformMat");
	glUniformMatrix4fv(WVPTransformLocation, 1, GL_FALSE, glm::value_ptr(WVPTransformMat));

	if (!pPointLight->attachLight2Shader(pPointLightShader, "uPointLight"))
	{
		fprintf(stderr,"Error: when attach point light to shader");
	}
 	pPointLightShader->setUniformInt("uPositionSampler", 0);
	pPointLightShader->setUniformInt("uNormalSampler", 1);

	pPointLightShader->setUniformVec3("uEyeWorldPos", gEyePos.x, gEyePos.y, gEyePos.z);
 	pPointLightShader->setUniformVec2("uScreenSize", WIN_WIDTH, WIN_HEIGHT);
	pPointLightShader->setUniformFloat("uSpecularIntensity", 0.5);
	pPointLightShader->setUniformFloat("uSpecularPower", 32.0);
}

void initDirectionalLightUniform()
{
	if (!pDirectionalLight->attachLight2Shader(pDirectionalLightShader, "uDirectionalLight"))
	{
		fprintf(stderr,"Error: when attach directional light to shader");
	}
	pDirectionalLightShader->setUniformInt("uPositionSampler", 0);
	pDirectionalLightShader->setUniformInt("uNormalSampler", 1);

	pDirectionalLightShader->setUniformVec3("uEyeWorldPos", gEyePos.x, gEyePos.y, gEyePos.z);
	pDirectionalLightShader->setUniformVec2("uScreenSize", WIN_WIDTH, WIN_HEIGHT);
	pDirectionalLightShader->setUniformFloat("uSpecularIntensity", 0.5);
	pDirectionalLightShader->setUniformFloat("uSpecularPower", 32.0);
}

void initPipeline()
{
	pGBuffer = new CFBO;
	pGBuffer->creatFBO(WIN_WIDTH, WIN_HEIGHT);
	pGBuffer->addBufferToFBO(DepthTex, 0);
	pGBuffer->addBufferToFBO(WorldTex, 1);
	pGBuffer->addBufferToFBO(DiffuseTex, 2);
	pGBuffer->addBufferToFBO(NormalTex, 3);
	pGBuffer->addBufferToFBO(TexcoordTex, 4);
	pGBuffer->addBufferToFBO(PointLightShadingTex, 5);
	pGBuffer->addBufferToFBO(DirectionalLightShadingTex, 6);
	pGBuffer->addBufferToFBO(CompositeShadingTex, 7);
}

void initShader()
{
	pGeomShader = new CShader;
	pGeomShader->addShader(GL_VERTEX_SHADER, "shader/geomV.glsl");
	pGeomShader->addShader(GL_FRAGMENT_SHADER, "shader/geomF.glsl");
	pGeomShader->compileShader();

	pStencilShader = new CShader;
	pStencilShader->addShader(GL_VERTEX_SHADER, "shader/lightV.glsl");
	pStencilShader->addShader(GL_FRAGMENT_SHADER, "shader/nullF.glsl");
	pStencilShader->compileShader();

	pPointLightShader = new CShader;
	pPointLightShader->addShader(GL_VERTEX_SHADER, "shader/lightV.glsl");
	pPointLightShader->addShader(GL_FRAGMENT_SHADER, "shader/pointLightF.glsl");
	pPointLightShader->compileShader();

	pDirectionalLightShader = new CShader;
	pDirectionalLightShader->addShader(GL_VERTEX_SHADER, "shader/drawScreenQuadV.glsl");
	pDirectionalLightShader->addShader(GL_FRAGMENT_SHADER, "shader/directionalLightF.glsl");
	pDirectionalLightShader->compileShader();

	pCompositeShader = new CShader;
	pCompositeShader->addShader(GL_VERTEX_SHADER, "shader/drawScreenQuadV.glsl");
	pCompositeShader->addShader(GL_FRAGMENT_SHADER, "shader/compositeF.glsl");
	pCompositeShader->compileShader();

	pOutputTextureShader = new CShader;
	pOutputTextureShader->addShader(GL_VERTEX_SHADER, "shader/drawScreenQuadV.glsl");
	pOutputTextureShader->addShader(GL_FRAGMENT_SHADER, "shader/outputTextureF.glsl");
	pOutputTextureShader->compileShader();

}

void initScene()
{
	pBaseSphere = new CMesh;
	pBaseSphere->loadMesh("model/sphere.obj");

	pModel = new CMesh;
	pModel->loadMesh("model/dragon.obj");

	pScreenQuad = new CQuad;
	pScreenQuad->setFullScreenQuad(WIN_WIDTH, WIN_HEIGHT);
}

void geomPass()
{
	GLenum DrawBuffer[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};
 	glDrawBuffers(5, DrawBuffer);

	pGeomShader->enable();
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	initGeometryUniform();

	pModel->render();

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	pGeomShader->disable();
}

void stencilPass()
{
	glDrawBuffer(GL_NONE);
	pStencilShader->enable();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 0, 0);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

	initStentcilUniform();
	pBaseSphere->render();

	pStencilShader->disable();
}

void pointLightShadingPass()
{
	glDrawBuffer(GL_COLOR_ATTACHMENT5);

	pPointLightShader->enable();
	glClear(GL_COLOR_BUFFER_BIT);
	initPointLightUniform();
	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, WorldTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, NormalTex);

	pBaseSphere->render();
	glCullFace(GL_BACK);

	pPointLightShader->disable();
}

void directionalLightShadingPass()
{
	glDrawBuffer(GL_COLOR_ATTACHMENT6);
	pDirectionalLightShader->enable();
	initDirectionalLightUniform();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, WorldTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, NormalTex);
	pScreenQuad->render();

	pDirectionalLightShader->disable();
}

void lightShadingPass()
{
	initLight();
	glEnable(GL_STENCIL_TEST);
	stencilPass();
	pointLightShadingPass();
	glDisable(GL_STENCIL_TEST);
	directionalLightShadingPass();
}

void compositePass()
{
	glDrawBuffer(GL_COLOR_ATTACHMENT7);
	pCompositeShader->enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	pCompositeShader->setUniformVec2("uScreenSize", WIN_WIDTH, WIN_HEIGHT);
	pCompositeShader->setUniformInt("uPointLightSampler", 0);
	pCompositeShader->setUniformInt("uDirectionalLightSampler", 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PointLightShadingTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, DirectionalLightShadingTex);

	pScreenQuad->render();

	pCompositeShader->disable();
}

void outputScreenPass()
{
	pOutputTextureShader->enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pOutputTextureShader->setUniformVec2("uScreenSize", WIN_WIDTH, WIN_HEIGHT);
	pOutputTextureShader->setUniformInt("uColorSampler", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, CompositeShadingTex);
	pScreenQuad->render();

	pOutputTextureShader->disable();
}

void RenderScene()
{
	pGBuffer->bindFBO();
	geomPass();
	lightShadingPass();
	compositePass();
	pGBuffer->banFBO();
	outputScreenPass();
}

int main(void)
{
	GLFWwindow *pWindow;
	if (!glfwInit()) return -1;

	pWindow = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Deferred Shading", NULL, NULL);

	if (!pWindow)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(pWindow);
	GLuint pRes = glewInit();
	if (pRes != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(pRes));
		return -1;
	}

	glfwSwapInterval(1);
	glfwSetKeyCallback(pWindow, key_callback);

//	glCullFace(GL_BACK);

	initScene();
	initShader();
	initPipeline();

	while (!glfwWindowShouldClose(pWindow))
	{
		RenderScene();
		glfwSwapBuffers(pWindow);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}