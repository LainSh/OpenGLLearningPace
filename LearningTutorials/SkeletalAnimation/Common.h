#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

const unsigned int MAX_BONES = 100; 

struct SDirLightLocation
{
	GLuint Color;
	GLuint AmbientIntensity;
	GLuint DiffuseIntensity;
	GLuint Direction;
}gDirectionLightLocation;

GLuint gBoneLocation[MAX_BONES];

struct SPointLightLocation
{
	GLuint m_BaseColorLocation;
	GLuint m_BaseAmbientIntensityLocation;
	GLuint m_BaseDiffuseIntensityLocation;
	GLuint m_PositionLocation;
	GLuint m_AttenConstantLocation;
	GLuint m_AttenLinearLocation;
	GLuint m_AttenExpLocation;
};


struct SPointLight 
{
	glm::vec3 m_Color;
	glm::vec3 m_Pos;
	float m_AmbientIntensity;
	float m_DiffuseIntensity;
	float m_AttenuationConstent;
	float m_AttenuationLinear;
	float m_AttenuationExp;

	SPointLight()
	{
		m_Pos = glm::vec3(0.0, 0.0, 0.0);
		m_Color = glm::vec3(0.0, 0.0, 0.0);
		m_AmbientIntensity = 0.01;
		m_DiffuseIntensity = 0.0;
		m_AttenuationConstent = 1.0;
		m_AttenuationLinear = 0.0;
		m_AttenuationExp = 0.0;
	}
};