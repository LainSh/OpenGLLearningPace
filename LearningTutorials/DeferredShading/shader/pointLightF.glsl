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

struct SPointLight
{
	SBaseLight Base;
	SAttenuation Atten;
	vec3 Position;
};

uniform SPointLight uPointLight;

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

vec4 calculatePointLight(vec3 vWorldPos, vec3 vNormal)
{
	vec3 LightDirection = vWorldPos - uPointLight.Position;
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);

	vec4 Color = calculateLightInternal(uPointLight.Base, LightDirection, vWorldPos, vNormal);

	float Attenuation = uPointLight.Atten.Constant +
						uPointLight.Atten.Linear * Distance +
						uPointLight.Atten.Exp * Distance * Distance;
	
	Attenuation = max(1.0, Attenuation);

	return (Color / Attenuation);
}

void main()
{
	vec2 TexCoord = calculateTexCoord();
	vec3 WorldPos = texture(uPositionSampler, TexCoord).xyz;
	vec3 Normal = texture(uNormalSampler, TexCoord).xyz;
	Normal = normalize(Normal);

//	if (WorldPos.z < 0) discard;

	FragColor = calculatePointLight(WorldPos, Normal);
//	FragColor = vec4(WorldPos, 1.0);
//	FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}