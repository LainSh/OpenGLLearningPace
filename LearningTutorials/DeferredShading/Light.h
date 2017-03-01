#pragma once
#include <glm/glm.hpp>
#include "Shader.h"

class CLight
{
public:
	enum ELightType
	{
		UNKNOWN_TYPE = 0,
		DIRECTIONAL_LIGHT,
		POINT_LIGHT,
		SPOT_LIGHT
	};
	
	struct SBaseLight
	{
		float m_Color[3];
		float m_AmbientIntensity;
		float m_DiffuseIntensity;

		SBaseLight() {}
		void setColor(float vRed, float vGreen, float vBlue)
		{
			m_Color[0] = vRed;	m_Color[1] = vGreen; m_Color[2] =vBlue;
		}
	};

	struct SAttenuation
	{
		float m_Constant;
		float m_Linear;
		float m_Exp;

		SAttenuation() {}
		SAttenuation(float vConstant, float vLinear, float vExp)
		{
			m_Constant = vConstant;
			m_Linear   = vLinear;
			m_Exp      = vExp;
		}
	};

public:
	CLight(void);
	~CLight(void);

	CLight(ELightType vLightType);

	bool setLightColor(float vRed, float vGreen, float vBlue);
	bool setAmbientIntensity(float vAmbientIntensity);
	bool setDiffuseIntensity(float vDiffuseIntensity);

	bool setLightPositionOrDirection(float vX, float vY, float vZ);
	bool setLightPositionOrDirection(const glm::vec3& vValue);
	bool setLightAttenuation(float vConstant, float vLinear, float vExp);
	bool attachLight2Shader(CShader *vShader, const std::string& vLightName); 

	glm::vec3 getLightPositionOrDirection() const;

private:
	ELightType m_Type;
	SBaseLight m_Base;
	SAttenuation m_Atten;
	float m_PositionOrDirection[3];
};