#version 430

out vec4 FragColor;

struct SBaseLight
{
	vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;
};

struct SAttenuation
{
	float Constant;
	float Linear;
	float Exp;
};

struct SDirecionalLight
{
	SBaseLight Base;
	vec3 Direction;
};

uniform SDirecionalLight uDirectionalLight;

uniform sampler2D   uPositionSampler;
uniform sampler2D   uNormalSampler;

uniform vec3		uEyeWorldPos;
uniform vec2		uScreenSize;
uniform	float		uSpecularIntensity;
uniform	float		uSpecularPower;

vec2 calculateTexCoord()
{
    return gl_FragCoord.xy / uScreenSize;
}

vec4 calculateLightInternal(SBaseLight vBaseLight, vec3 vLightDirection, vec3 vWorldPos, vec3 vNormal)
{
	vec4 AmbientColor  = vec4(vBaseLight.Color * vBaseLight.AmbientIntensity, 1.0);
	vec4 DiffuseColor  = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 SpecularColor = vec4(0.0, 0.0, 0.0, 0.0);

	float DiffuseFactor = dot(vNormal, -vLightDirection);
	if (DiffuseFactor > 0.0)
	{
		DiffuseColor = vec4(vBaseLight.Color * vBaseLight.DiffuseIntensity * DiffuseFactor, 1.0);

		vec3 Vertex2Eye = normalize(uEyeWorldPos - vWorldPos);
		vec3 LightReflect = normalize(reflect(vLightDirection, vNormal));
		float SpecularFactor = dot(Vertex2Eye, LightReflect);
		if (SpecularFactor > 0.0)
		{
			SpecularFactor = pow(SpecularFactor, uSpecularPower);
			SpecularColor  = vec4(vBaseLight.Color * uSpecularIntensity * SpecularFactor, 1.0);
		}
	}

	return (AmbientColor + DiffuseColor + SpecularColor);
}

vec4 calculateDirectionalLight(vec3 vWorldPos, vec3 vNormal)
{
	return calculateLightInternal(uDirectionalLight.Base, uDirectionalLight.Direction, vWorldPos, vNormal);
}

void main()
{
	vec2 TexCoord = calculateTexCoord();
	vec3 WorldPos = texture(uPositionSampler, TexCoord).xyz;
	vec3 Normal = texture(uNormalSampler, TexCoord).xyz;
	Normal = normalize(Normal);

//	if (WorldPos.z < 0) discard;

	FragColor = calculateDirectionalLight(WorldPos, Normal);
}