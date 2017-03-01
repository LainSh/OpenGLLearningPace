#include "Light.h"

CLight::CLight(void)
{
}

CLight::CLight(ELightType vLightType)
{
	m_Type = vLightType;
}


CLight::~CLight(void)
{
}

//******************************************************************
//FUNCTION:
bool CLight::setLightColor(float vRed, float vGreen, float vBlue)
{
	m_Base.setColor(vRed, vGreen, vBlue);
	return true;
}

//******************************************************************
//FUNCTION:
bool CLight::setAmbientIntensity(float vAmbientIntensity)
{
	m_Base.m_AmbientIntensity = vAmbientIntensity;
	return true;
}

//******************************************************************
//FUNCTION:
bool CLight::setDiffuseIntensity(float vDiffuseIntensity)
{
	m_Base.m_DiffuseIntensity = vDiffuseIntensity;
	return true;
}

//******************************************************************
//FUNCTION:
bool CLight::setLightPositionOrDirection(float vX, float vY, float vZ)
{
	m_PositionOrDirection[0] = vX;
	m_PositionOrDirection[1] = vY;
	m_PositionOrDirection[2] = vZ;
	return true;
}

//******************************************************************
//FUNCTION:
bool CLight::setLightPositionOrDirection(const glm::vec3& vValue)
{
	m_PositionOrDirection[0] = vValue.x;
	m_PositionOrDirection[1] = vValue.y;
	m_PositionOrDirection[2] = vValue.z;
	return true;
}

//******************************************************************
//FUNCTION:
bool CLight::setLightAttenuation(float vConstant, float vLinear, float vExp)
{
	SAttenuation Atten(vConstant, vLinear, vExp);
	m_Atten = Atten;
	return true;
}

//******************************************************************
//FUNCTION:
bool CLight::attachLight2Shader(CShader *vShader, const std::string& vLightName)
{
	std::string BaseStr;
	std::string PositionOrDirection;

	BaseStr.append(vLightName);
	switch (m_Type)
	{
	case DIRECTIONAL_LIGHT:
		PositionOrDirection = BaseStr + ".Direction";
		break;
	case POINT_LIGHT:
		PositionOrDirection = BaseStr + ".Position";
		break;
	}
	std::string BaseColor = BaseStr + ".Base.Color";
	std::string BaseAmbientIntensity = BaseStr + ".Base.AmbientIntensity";
	std::string BaseDiffuseIntensity = BaseStr + ".Base.DiffuseIntensity";

	std::string AttenConstant = BaseStr + ".Atten.Constant";
	std::string AttenLinear   = BaseStr + ".Atten.Linear";
	std::string AttenExp      = BaseStr + ".Atten.Exp";

	vShader->enable();
	vShader->setUniformVec3(BaseColor, m_Base.m_Color[0], m_Base.m_Color[1], m_Base.m_Color[2]);
	vShader->setUniformFloat(BaseAmbientIntensity, m_Base.m_AmbientIntensity);
	vShader->setUniformFloat(BaseDiffuseIntensity, m_Base.m_DiffuseIntensity);
	vShader->setUniformVec3(PositionOrDirection, m_PositionOrDirection[0], m_PositionOrDirection[1], m_PositionOrDirection[2]);
	if (m_Type == POINT_LIGHT)
	{
		vShader->setUniformFloat(AttenConstant, m_Atten.m_Constant);
		vShader->setUniformFloat(AttenLinear, m_Atten.m_Linear);
		vShader->setUniformFloat(AttenExp, m_Atten.m_Exp);
	}
//	vShader->disable();

	return vShader->isUniformValidated();
}

//******************************************************************
//FUNCTION:
glm::vec3 CLight::getLightPositionOrDirection() const
{
	return glm::vec3(m_PositionOrDirection[0], m_PositionOrDirection[1], m_PositionOrDirection[2]);
}
