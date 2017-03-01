#version 430                                                                 

layout (location = 0) in vec3 Position; 

uniform mat4 uWVPTransformMat;

void main()
{          
    gl_Position = uWVPTransformMat * vec4(Position, 1.0);
}
