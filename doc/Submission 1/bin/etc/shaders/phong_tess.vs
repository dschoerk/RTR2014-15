#version 430 core

layout (location = 0) in vec3 Position; 
layout (location = 1) in vec3 Normal; 
layout (location = 3) in vec2 Texcoord; 

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexcoord;

uniform mat4 gWVP;
uniform mat4 gWorld;
uniform mat3 gNormal;

void main()
{
    vPosition = Position;
    vNormal = Normal;
	vTexcoord = Texcoord;
	//gl_Position = gWVP * vec4(Position,1);
}