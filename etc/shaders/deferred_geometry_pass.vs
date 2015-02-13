#version 330 

layout (location = 0) in vec3 Position; 
layout (location = 1) in vec3 Normal; 
layout (location = 3) in vec2 Texcoord; 

uniform mat4 gWVP;
uniform mat4 gWorld;
uniform mat3 gNormal;

out vec2 TexCoord0; 
out vec3 Normal0; 
out vec3 WorldPos0; 

void main()
{ 
    gl_Position = gWVP * vec4(Position, 1.0); 
    //Normal0 = (gWorld * vec4(Normal, 0.0)).xyz; 
	Normal0 = gNormal * Normal;
    WorldPos0 = vec3(gWorld * vec4(Position, 1.0));
	TexCoord0 = Texcoord;
}