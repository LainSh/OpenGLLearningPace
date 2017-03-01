#version 430

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;

uniform mat4 uWVPTransformMat;
uniform mat4 uWTransformMat;
uniform mat4 uNormalMat;

out vec3 WorldPosV;
out vec3 NormalV;
out vec2 TexCoordV;

void main()
{
	gl_Position = uWVPTransformMat * vec4(Position, 1.0);
	NormalV = (uNormalMat * vec4(Normal, 0.0)).xyz;
	WorldPosV = (uWTransformMat * vec4(Position, 1.0)).xyz;
	TexCoordV = TexCoord;
}