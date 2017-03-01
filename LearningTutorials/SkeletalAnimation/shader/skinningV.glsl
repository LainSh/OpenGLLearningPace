#version 430

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

out vec2 TexCoordV;
out vec3 NormalV;
out vec3 WorldPosV;

const int gMaxBones = 100;

uniform mat4 uWVPTransformMat;
uniform mat4 uWTransformMat;
uniform mat4 uNormalMat;
uniform mat4 uBones[gMaxBones];

void main()
{
	mat4 BoneTransform = uBones[BoneIDs[0]] * Weights[0];
	BoneTransform     += uBones[BoneIDs[1]] * Weights[1];
    BoneTransform     += uBones[BoneIDs[2]] * Weights[2];
    BoneTransform     += uBones[BoneIDs[3]] * Weights[3];

	vec4 PosL = BoneTransform * vec4(Position, 1.0);
	gl_Position = uWVPTransformMat * PosL;
	TexCoordV = TexCoord;
	vec4 NormalL = BoneTransform * vec4(Normal, 0.0);
	NormalV = (uNormalMat * NormalL).xyz;
	WorldPosV = (uWTransformMat * PosL).xyz;
}