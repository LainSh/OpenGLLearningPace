#version 430

in vec3 WorldPosV;
in vec3 NormalV;
in vec2 TexCoordV;

const int LightsAmount = 1;

struct SBaseLight
{
	vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;
};

struct SDirectionalLight
{
	SBaseLight Base;
	vec3 Direction;
};

struct SAttenuation
{
	float Constant;
	float Linear;
	float Exp;
};

struct SPointLight
{
	SBaseLight Base;
	SAttenuation Atten;
	vec3 Position;
};

uniform int               uPointLightNum;
uniform SDirectionalLight uDirectionalLight;
uniform SPointLight       uPointLight[LightsAmount];
uniform vec3              uEyeWorldPos;
uniform float			  uSpecularIntensity;
uniform float			  uSpecularPower;
uniform sampler2D         uTexSampler;

out vec4 FragColor;


vec4 calculateLightInternal(SBaseLight vLight, vec3 vLightDirection, vec3 vNormal)
{
	vec4 AmbientColor = vec4(vLight.Color * vLight.AmbientIntensity, 1.0);

	vec4 DiffuseColor = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 SpecularColor = vec4(0.0, 0.0, 0.0, 0.0);

	float DiffuseFactory = dot(vNormal, -vLightDirection);
	if(DiffuseFactory > 0)
	{
		DiffuseColor = vec4(vLight.Color * vLight.DiffuseIntensity * DiffuseFactory, 1.0);

		vec3 VertexToEye = normalize(uEyeWorldPos - WorldPosV);
		vec3 LightReflect = normalize(reflect(vLightDirection, vNormal));
		float SpecularFactory = dot(VertexToEye, LightReflect);
		if(SpecularFactory > 0)
		{
			SpecularFactory = pow(SpecularFactory, uSpecularPower);
			SpecularColor = vec4(vLight.Color * uSpecularIntensity * SpecularFactory, 1.0);
		}
	}

	return (AmbientColor + DiffuseColor + SpecularColor);
}

vec4 calculateDirectionalLight(SDirectionalLight vDirectionalLight, vec3 vNormal)
{
	return (calculateLightInternal(vDirectionalLight.Base, vDirectionalLight.Direction, vNormal));
}

vec4 calculatePointLight(SPointLight vPointLight, vec3 vNormal)
{
	vec3 LightDirection = WorldPosV - vPointLight.Position;
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);

	vec4 Color = calculateLightInternal(vPointLight.Base, LightDirection, vNormal);
	float Attenuation = vPointLight.Atten.Constant +
						vPointLight.Atten.Linear * Distance +
						vPointLight.Atten.Exp * Distance * Distance;

	return (Color / Attenuation);
}

void main()
{
	vec3 Normal = normalize(NormalV);

	vec4 TotalLight = calculateDirectionalLight(uDirectionalLight, Normal);

	for(int i=0; i<uPointLightNum; i++)
	{
		TotalLight += calculatePointLight(uPointLight[i], Normal);
	}

	FragColor = texture(uTexSampler, TexCoordV.xy) * TotalLight;
//	FragColor = TotalLight;
}