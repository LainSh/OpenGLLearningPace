#version 430

in vec3 WorldPosV;
in vec3 NormalV;
in vec2 TexCoordV;

layout (location = 0) out vec3 DepthOut;
layout (location = 1) out vec3 PositionOut;
layout (location = 2) out vec3 DiffuseOut;
layout (location = 3) out vec3 NormalOut;
layout (location = 4) out vec2 TexCoordOut;

uniform sampler2D uColorSampler;

void main()
{
	float Depth = gl_FragCoord.z; 
	DepthOut = vec3(Depth , Depth, Depth);
	PositionOut = WorldPosV;
	DiffuseOut = texture(uColorSampler, TexCoordV).xyz;
	NormalOut = normalize(NormalV);
	TexCoordOut = TexCoordV;
}
