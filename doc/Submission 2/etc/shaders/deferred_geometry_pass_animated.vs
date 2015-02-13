#version 440 

layout (location = 0) in vec3 Position; 
layout (location = 1) in vec3 Normal; 
layout (location = 3) in vec2 Texcoord; 
layout (location = 4) in ivec4 BoneId; 
layout (location = 5) in vec4 BoneWeight; 

uniform mat4 gWVP;
uniform mat4 gWorld;
uniform mat3 gNormal;
uniform mat4 gBoneMatrix[40];

out vec2 TexCoord0; 
out vec3 Normal0; 
out vec3 WorldPos0; 
out vec3 Color0;

void main()
{ 
	// combine final transformation
	//uvec4 bid = uvec4((unsigned int)BoneId[0],(unsigned int)BoneId[1],(unsigned int)BoneId[2],(unsigned int)BoneId[3]) ;
	ivec4 bid = BoneId;
	mat4 transform  = gBoneMatrix[bid[0]] * BoneWeight[0];
	transform += gBoneMatrix[bid[1]] * BoneWeight[1];
	transform += gBoneMatrix[bid[2]] * BoneWeight[2];
	transform += gBoneMatrix[bid[3]] * BoneWeight[3];
	
    gl_Position = gWVP * transform * vec4(Position, 1.0); 
	Normal0 = gNormal * Normal;
    WorldPos0 = vec3(gWorld * transform * vec4(Position, 1.0));
	TexCoord0 = Texcoord;
	
	Color0 = vec3(1,0,0);
}